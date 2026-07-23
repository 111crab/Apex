// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ApexAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ApexVitalAttributeSet.generated.h"

/**
 * UApexVitalAttributeSet — Apex 生命与法力资源属性集。
 *
 * 资源属性由服务端权威结算、复制到客户端。
 * 客户端通过 RepNotify 接收变化并刷新 UI。
 *
 * 复制策略：
 * - Health / MaxHealth：全端复制（COND_None），所有客户端都需要看到血量变化。
 * - Mana / MaxMana：仅拥有者复制（COND_OwnerOnly），其他玩家不需要知道精确法力值。
 *
 * 钳制规则：
 * - 0 <= Health <= MaxHealth
 * - 1 <= MaxHealth（防止除以零和空生命上限）
 * - 0 <= Mana <= MaxMana
 * - 0 <= MaxMana
 * - 最大值降低且当前值超过新上限时，通过 ASC 属性修改入口同步压低当前值。
 *   最大值提高时不自动补满，不保持比例。
 */
UCLASS(BlueprintType)
class APEX_API UApexVitalAttributeSet : public UApexAttributeSet
{
	GENERATED_BODY()

public:
	UApexVitalAttributeSet();

	APEX_ATTRIBUTE_ACCESSORS(UApexVitalAttributeSet, Health)
	APEX_ATTRIBUTE_ACCESSORS(UApexVitalAttributeSet, MaxHealth)
	APEX_ATTRIBUTE_ACCESSORS(UApexVitalAttributeSet, Mana)
	APEX_ATTRIBUTE_ACCESSORS(UApexVitalAttributeSet, MaxMana)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	/** 统一钳制函数：将值限制在 [Min, Max] 范围内 */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	// ========================================================================
	// 属性
	// ========================================================================

	/** 当前生命 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Apex|Vital", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;

	/** 最大生命 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Apex|Vital", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

	/** 当前法力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Apex|Vital", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Mana;

	/** 最大法力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Apex|Vital", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxMana;

	// ========================================================================
	// RepNotify
	// ========================================================================

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
};
