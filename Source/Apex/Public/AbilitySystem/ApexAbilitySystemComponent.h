// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ApexAbilitySystemComponent.generated.h"

/**
 * UApexAbilitySystemComponent — Apex GAS 技能系统组件。
 *
 * 设计原则：
 * - 输入路由：每帧从 PlayerController 接收 InputTag 队列，匹配 Dynamic Source Tags 激活 Spec。
 * - Pressed/Released 通过 InvokeReplicatedEvent 支持 WaitInputPress/WaitInputRelease。
 * - 技能授予、GE 应用等均由此组件统一管理。
 */
UCLASS()
class APEX_API UApexAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// ========================================================================
	// 输入路由
	// ========================================================================

	/** 处理单次按下：匹配 Spec、尝试激活、加入 Pressed/Held 队列 */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	/** 处理单次释放：加入 Released 队列、从 Held 移除 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/** 每帧推进输入队列 */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	/** 清空全部输入队列 */
	void ClearAbilityInput();

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

private:
	/** 本次帧内按下的 Spec Handle */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** 当前仍被按住的 Spec Handle（松开后移除） */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	/** 本次帧内松开的 Spec Handle */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
