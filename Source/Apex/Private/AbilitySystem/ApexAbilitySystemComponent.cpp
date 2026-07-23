// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ApexGameplayAbility.h"
#include "GameplayTags/ApexGameplayTags.h"

namespace
{
	/** 检查 InputTag 是否为 InputTag.Ability 的具体子 Tag（根 Tag 不能充当输入槽）。 */
	bool IsValidAbilityInputTag(const FGameplayTag& Tag)
	{
		return Tag.IsValid()
			&& Tag.MatchesTag(ApexGameplayTags::InputTag_Ability)
			&& !Tag.MatchesTagExact(ApexGameplayTags::InputTag_Ability);
	}
}

// ============================================================================
// 输入路由
// ============================================================================

void UApexAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	// 拒绝根域外 Tag。
	if (!IsValidAbilityInputTag(InputTag))
	{
		return;
	}

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		// 只入队，不在此遍历内调用 TryActivateAbility。
		InputPressedSpecHandles.AddUnique(Spec.Handle);
		InputHeldSpecHandles.AddUnique(Spec.Handle);
	}
}

void UApexAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!IsValidAbilityInputTag(InputTag))
	{
		return;
	}

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		InputReleasedSpecHandles.AddUnique(Spec.Handle);
		InputHeldSpecHandles.Remove(Spec.Handle);
	}
}

void UApexAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// ---- Held：收集 WhileInputActive 未激活能力 ----
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		if (AbilitiesToActivate.Contains(Handle))
		{
			continue;
		}

		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || Spec->IsActive())
		{
			continue;
		}

		if (const UApexGameplayAbility* GA = Cast<UApexGameplayAbility>(Spec->Ability))
		{
			if (GA->GetActivationPolicy() == EApexAbilityActivationPolicy::WhileInputActive)
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	// ---- Pressed：设置 InputPressed，向已激活能力发事件，收集 OnInputTriggered ----
	for (const FGameplayAbilitySpecHandle& Handle : InputPressedSpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || !Spec->Ability)
		{
			continue;
		}

		Spec->InputPressed = true;

		if (Spec->IsActive())
		{
			AbilitySpecInputPressed(*Spec);
		}
		else
		{
			const UApexGameplayAbility* GA = Cast<UApexGameplayAbility>(Spec->Ability);
			if (GA && GA->GetActivationPolicy() == EApexAbilityActivationPolicy::OnInputTriggered)
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	// ---- 统一激活 ----
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	// ---- Released：清除 InputPressed，向已激活能力发事件 ----
	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedSpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || !Spec->Ability)
		{
			continue;
		}

		Spec->InputPressed = false;

		if (Spec->IsActive())
		{
			AbilitySpecInputReleased(*Spec);
		}
	}

	// 每帧结束清空 Pressed/Released；Held 保留到松开。
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UApexAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

// ============================================================================
// GAS 输入事件（支持 WaitInputPress/WaitInputRelease 等 AbilityTask）
// ============================================================================

void UApexAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// 只对已激活的能力发送 Generic Replicated Event。
	if (!Spec.IsActive())
	{
		return;
	}

	// PredictionKey 优先从 PrimaryInstance 获取；fallback 为 NonInstanced legacy。
	FPredictionKey PredictionKey;
	if (UGameplayAbility* PrimaryInstance = Spec.GetPrimaryInstance())
	{
		PredictionKey = PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
	}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	else
	{
		PredictionKey = Spec.ActivationInfo.GetActivationPredictionKey();
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, PredictionKey);
}

void UApexAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (!Spec.IsActive())
	{
		return;
	}

	FPredictionKey PredictionKey;
	if (UGameplayAbility* PrimaryInstance = Spec.GetPrimaryInstance())
	{
		PredictionKey = PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
	}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	else
	{
		PredictionKey = Spec.ActivationInfo.GetActivationPredictionKey();
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, PredictionKey);
}
