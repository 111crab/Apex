// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ApexAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 * UApexAnimInstance — Apex 项目 AnimInstance 基类。
 *
 * 职责：将 CharacterMovement 玩法移动状态翻译为 AnimBP 可读取的稳定缓存值。
 *
 * 设计原因：
 * - 在 C++ 层的 NativeUpdateAnimation 中一次性读取 CharacterMovement UObject 状态并
 *   缓存为基础类型（float / bool），AnimBP EventGraph 不再需要每帧 Cast Character，
 *   减少蓝图节点开销并统一数据来源。
 * - 缓存 ACharacter（而非 AApexPlayerCharacter），使 AI、敌人等其他 ACharacter
 *   派生类也能复用该 AnimInstance。
 * - Owner 在编辑器预览或初始化时序中可能为空，这是合法状态，不做 check()。
 * - 本类不控制 CharacterMovement、不调用 Jump、不激活 Ability、不播放 Montage。
 */
UCLASS(Transient, Blueprintable)
class APEX_API UApexAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// --- AnimBP 只读 Locomotion 状态 ---
	// 由 NativeUpdateAnimation 每帧更新，AnimBP 只读不写。

	/** 地面速度（Velocity 2D 长度），驱动 Locomotion BlendSpace */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation|Locomotion")
	float GroundSpeed = 0.f;

	/** 垂直速度（Velocity.Z），区分上升跳跃与下落 */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation|Locomotion")
	float VerticalSpeed = 0.f;

	/** 是否有加速度输入，区分主动移动与惯性滑动 */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation|Locomotion")
	bool bHasAcceleration = false;

	/** 是否处于下落/跳跃空中状态 */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation|Locomotion")
	bool bIsFalling = false;

	// --- UAnimInstance Overrides ---

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	// --- 内部缓存引用（不暴露给蓝图）---

	/** 缓存的拥有者 Character，供内部状态计算使用 */
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	/** 缓存的 CharacterMovementComponent，避免每帧动态查询 */
	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	// --- 内部 Helper ---

	/** 缓存 OwningCharacter 与 CharacterMovementComponent；失败时清空两者 */
	void CacheOwnerReferences();

	/** 将全部 Locomotion 状态重置为安全默认值 */
	void ResetLocomotionState();
};
