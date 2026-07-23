// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ApexPlayerCharacter.h"
#include "Player/ApexPlayerState.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Input/ApexAbilityInputConfig.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Apex.h"

AApexPlayerCharacter::AApexPlayerCharacter()
{
	// --- 相机摇杆（SpringArm）---
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 400.f;
	CameraSpringArm->bUsePawnControlRotation = true;

	// --- 跟随相机 ---
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = false;
}

// ============================================================================
// GAS 初始化
// ============================================================================

UAbilitySystemComponent* AApexPlayerCharacter::GetAbilitySystemComponent() const
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	return ApexPS ? ApexPS->GetAbilitySystemComponent() : nullptr;
}

void AApexPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAbilitySystemActorInfo();

	// 服务器：ActorInfo 初始化后授予 CoreAbilitySet。
	GrantCoreAbilitySet();
}

void AApexPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 客户端只初始化 ActorInfo，不执行授予（AbilitySpec 通过 GAS 复制到达）。
	InitializeAbilitySystemActorInfo();
}

void AApexPlayerCharacter::InitializeAbilitySystemActorInfo()
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	if (!ApexPS) return;

	UApexAbilitySystemComponent* ASC = ApexPS->GetApexAbilitySystemComponent();
	if (!ASC) return;

	ASC->InitAbilityActorInfo(ApexPS, this);

	UE_LOG(LogApex, Log, TEXT("[AApexPlayerCharacter] InitAbilityActorInfo: Owner=%s, Avatar=%s, LocalRole=%d"),
		*GetNameSafe(ApexPS), *GetNameSafe(this), static_cast<int32>(GetLocalRole()));
}

// ============================================================================
// Ability 授予与撤销
// ============================================================================

void AApexPlayerCharacter::GrantCoreAbilitySet()
{
	if (!CoreAbilitySet)
	{
		UE_LOG(LogApex, Log, TEXT("[AApexPlayerCharacter] 无 CoreAbilitySet，跳过授予。"));
		return;
	}

	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	if (!ApexPS) return;

	UApexAbilitySystemComponent* ASC = ApexPS->GetApexAbilitySystemComponent();
	if (!ASC) return;

	// 先撤销已有 Handle，避免重复 Possess 造成重复 Spec。
	RemoveCoreAbilitySet();

	CoreAbilitySet->GiveToAbilitySystem(ASC, &CoreAbilitySetHandles);
}

void AApexPlayerCharacter::RemoveCoreAbilitySet()
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	if (!ApexPS) return;

	UApexAbilitySystemComponent* ASC = ApexPS->GetApexAbilitySystemComponent();
	if (!ASC) return;

	// 输入队列是本地运行时状态，客户端和服务端均须清空。
	// 否则 UnPossessed 时按键仍 Held 的旧 Handle 会残留在 PlayerState ASC，
	// 污染重生或换英雄后的输入状态。
	ASC->ClearAbilityInput();

	// TakeFromAbilitySystem 内部使用 IsOwnerActorAuthoritative 只在权威端清除 Spec。
	CoreAbilitySetHandles.TakeFromAbilitySystem(ASC);
}

void AApexPlayerCharacter::UnPossessed()
{
	// 权威端：PossessedBy 授予的能力在此撤销。
	RemoveCoreAbilitySet();
	Super::UnPossessed();
}

void AApexPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveCoreAbilitySet();
	Super::EndPlay(EndPlayReason);
}

// ============================================================================
// Ability 输入路由
// ============================================================================

void AApexPlayerCharacter::InputAbilityTagPressed(FGameplayTag InputTag)
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	if (!ApexPS) return;

	UApexAbilitySystemComponent* ASC = ApexPS->GetApexAbilitySystemComponent();
	if (ASC)
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AApexPlayerCharacter::InputAbilityTagReleased(FGameplayTag InputTag)
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	if (!ApexPS) return;

	UApexAbilitySystemComponent* ASC = ApexPS->GetApexAbilitySystemComponent();
	if (ASC)
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

// ============================================================================
// 输入
// ============================================================================

void AApexPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogApex, Error, TEXT("AApexPlayerCharacter::SetupPlayerInputComponent - '%s' 未找到 EnhancedInputComponent！"), *GetNameSafe(this));
		return;
	}

	// --- Ability 输入：遍历 AbilityInputConfig 绑定 ---
	if (AbilityInputConfig)
	{
		for (const FApexAbilityInputAction& Action : AbilityInputConfig->GetAbilityInputActions())
		{
			if (!Action.InputAction || !Action.InputTag.IsValid())
			{
				continue;
			}

			EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Started, this,
				&AApexPlayerCharacter::InputAbilityTagPressed, Action.InputTag);
			EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this,
				&AApexPlayerCharacter::InputAbilityTagReleased, Action.InputTag);
			EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Canceled, this,
				&AApexPlayerCharacter::InputAbilityTagReleased, Action.InputTag);
		}
	}

	// --- 跳跃：Started -> DoJumpStart, Completed -> DoJumpEnd ---
	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AApexPlayerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AApexPlayerCharacter::DoJumpEnd);
	}

	// --- 移动：Triggered -> Move ---
	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AApexPlayerCharacter::Move);
	}

	// --- 手柄视角：Triggered -> Look ---
	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AApexPlayerCharacter::Look);
	}

	// --- 鼠标视角：Triggered -> Look ---
	if (MouseLookAction)
	{
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AApexPlayerCharacter::Look);
	}
}

void AApexPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AApexPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AApexPlayerCharacter::DoMove(float Right, float Forward)
{
	AController* PlayerController = GetController();
	if (!PlayerController) return;

	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0.0, Rotation.Yaw, 0.0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, Forward);
	AddMovementInput(RightDirection, Right);
}

void AApexPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (!GetController()) return;
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void AApexPlayerCharacter::DoJumpStart()
{
	Jump();
}

void AApexPlayerCharacter::DoJumpEnd()
{
	StopJumping();
}
