// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ApexPlayerCharacter.h"
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
	CameraSpringArm->TargetArmLength = 400.f;           // 默认相机距离
	CameraSpringArm->bUsePawnControlRotation = true;    // SpringArm 跟随 Controller 旋转

	// --- 跟随相机 ---
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 相机本身不额外旋转，由 SpringArm 控制

	// 不开启 Tick。
	PrimaryActorTick.bCanEverTick = false;
}

void AApexPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 尝试转换为 Enhanced Input Component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogApex, Error, TEXT("AApexPlayerCharacter::SetupPlayerInputComponent - '%s' 未找到 EnhancedInputComponent！"), *GetNameSafe(this));
		return;
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

// --- 输入回调 ---

void AApexPlayerCharacter::Move(const FInputActionValue& Value)
{
	// Enhanced Input 值为 FVector2D: X=Right, Y=Forward
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AApexPlayerCharacter::Look(const FInputActionValue& Value)
{
	// Enhanced Input 值为 FVector2D: X=Yaw, Y=Pitch
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

// --- 蓝图可调用输入处理 ---

void AApexPlayerCharacter::DoMove(float Right, float Forward)
{
	AController* PlayerController = GetController();
	if (!PlayerController)
	{
		return;
	}

	// 使用 Controller 的 Yaw 旋转计算世界空间方向
	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0.0, Rotation.Yaw, 0.0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, Forward);
	AddMovementInput(RightDirection, Right);
}

void AApexPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (!GetController())
	{
		return;
	}

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
