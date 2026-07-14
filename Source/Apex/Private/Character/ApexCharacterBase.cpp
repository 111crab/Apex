// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ApexCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AApexCharacterBase::AApexCharacterBase()
{
	// --- 胶囊体默认尺寸（与 UE 第三人称模板一致）---
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// --- 角色旋转不跟随 Controller，只影响 Camera ---
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// --- 基础移动默认值（非技能逻辑，仅定义通用移动手感）---
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;           // 角色朝向移动方向
	Movement->RotationRate = FRotator(0.f, 500.f, 0.f); // 转向速率
	Movement->JumpZVelocity = 500.f;                      // 跳跃初速度
	Movement->AirControl = 0.35f;                         // 空中控制系数
	Movement->MaxWalkSpeed = 500.f;                       // 最大步行速度
	Movement->MinAnalogWalkSpeed = 20.f;                  // 最小模拟步行速度
	Movement->BrakingDecelerationWalking = 2000.f;        // 步行刹车减速度
	Movement->BrakingDecelerationFalling = 1500.f;        // 下落刹车减速度

	// 本次不开启 Tick，无需每帧更新。
	PrimaryActorTick.bCanEverTick = false;
}
