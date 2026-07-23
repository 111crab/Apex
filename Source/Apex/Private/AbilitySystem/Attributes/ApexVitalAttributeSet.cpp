// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Attributes/ApexVitalAttributeSet.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UApexVitalAttributeSet::UApexVitalAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
	, Mana(100.0f)
	, MaxMana(100.0f)
{
}

// ============================================================================
// 属性访问器注册
// ============================================================================


// ============================================================================
// 网络复制
// ============================================================================

void UApexVitalAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 血量：全端可见，所有客户端都需要看到血量变化。
	DOREPLIFETIME_CONDITION_NOTIFY(UApexVitalAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UApexVitalAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	// 法力：仅拥有者可见，不暴露给敌方。
	DOREPLIFETIME_CONDITION_NOTIFY(UApexVitalAttributeSet, Mana, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UApexVitalAttributeSet, MaxMana, COND_OwnerOnly, REPNOTIFY_Always);
}

// ============================================================================
// PreAttributeBaseChange — BaseValue 变化前钳制
// ============================================================================

void UApexVitalAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// BaseValue 和 CurrentValue 都需要钳制：
	// - BaseValue：Instant GE 修改 BaseValue，若无钳制可能写入非法值。
	// - CurrentValue：Duration GE Modifier 聚合后可能溢出，见 PreAttributeChange。
	ClampAttribute(Attribute, NewValue);
}

// ============================================================================
// PreAttributeChange — CurrentValue 变化前钳制
// ============================================================================

void UApexVitalAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

// ============================================================================
// PostAttributeChange — 最大值联动（当前值 > 新上限时压低）
// ============================================================================

void UApexVitalAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 最大值降低且当前值超过新上限时，通过 ASC 属性修改入口把当前值覆盖到新上限。
	// 例如 MaxHealth 从 200 降到 150，Health 当前为 180，则 Health 被压低到 150。
	// 这必须在 PostAttributeChange 中处理，因为此时新 BaseValue 已生效，
	// 且我们使用 ASC->SetNumericAttributeBase() 安全写入，避免绕过 PreAttributeBaseChange 钳制。

	// 最大值提高时不自动补满，也不保持比例。
	if (Attribute == GetMaxHealthAttribute() && NewValue < OldValue)
	{
		const float CurrentHealth = GetHealth();
		if (CurrentHealth > NewValue)
		{
			UApexAbilitySystemComponent* ASC = GetApexAbilitySystemComponent();
			if (ASC)
			{
				ASC->SetNumericAttributeBase(GetHealthAttribute(), NewValue);
			}
		}
	}
	else if (Attribute == GetMaxManaAttribute() && NewValue < OldValue)
	{
		const float CurrentMana = GetMana();
		if (CurrentMana > NewValue)
		{
			UApexAbilitySystemComponent* ASC = GetApexAbilitySystemComponent();
			if (ASC)
			{
				ASC->SetNumericAttributeBase(GetManaAttribute(), NewValue);
			}
		}
	}
}

// ============================================================================
// ClampAttribute — 统一钳制
// ============================================================================

void UApexVitalAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

// ============================================================================
// RepNotify
// ============================================================================

void UApexVitalAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UApexVitalAttributeSet, Health, OldValue);
}

void UApexVitalAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UApexVitalAttributeSet, MaxHealth, OldValue);
}

void UApexVitalAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UApexVitalAttributeSet, Mana, OldValue);
}

void UApexVitalAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UApexVitalAttributeSet, MaxMana, OldValue);
}
