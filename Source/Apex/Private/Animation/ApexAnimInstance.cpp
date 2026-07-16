// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/ApexAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UApexAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 初始化时缓存 Owner 引用；Owner 可能为空（编辑器预览），这是合法状态。
	CacheOwnerReferences();
}

void UApexAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 如果任一缓存引用无效或 Pawn 已切换，重新缓存。
	if (!::IsValid(OwningCharacter.Get())
		|| !::IsValid(CharacterMovementComponent.Get())
		|| OwningCharacter != TryGetPawnOwner())
	{
		CacheOwnerReferences();
	}

	// 缓存后仍无效（无 Owner 或无 MovementComponent），重置状态后安全退出。
	if (!::IsValid(OwningCharacter.Get()) || !::IsValid(CharacterMovementComponent.Get()))
	{
		ResetLocomotionState();
		return;
	}

	// --- 从 CharacterMovement 读取并缓存基础类型状态 ---

	const FVector Velocity = CharacterMovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();
	VerticalSpeed = Velocity.Z;

	bHasAcceleration = CharacterMovementComponent->GetCurrentAcceleration().SizeSquared2D() > KINDA_SMALL_NUMBER;
	bIsFalling = CharacterMovementComponent->IsFalling();
}

void UApexAnimInstance::CacheOwnerReferences()
{
	// 尝试从 Pawn Owner 获取 ACharacter。
	ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	OwningCharacter = Character;

	if (::IsValid(OwningCharacter.Get()))
	{
		CharacterMovementComponent = OwningCharacter->GetCharacterMovement();
	}
	else
	{
		// Owner 无效时清空所有内部引用（编辑器预览、未生成 Pawn 等场景）。
		CharacterMovementComponent = nullptr;
	}
}

void UApexAnimInstance::ResetLocomotionState()
{
	GroundSpeed = 0.f;
	VerticalSpeed = 0.f;
	bHasAcceleration = false;
	bIsFalling = false;
}
