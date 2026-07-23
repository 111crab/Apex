// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ApexAbilitySet.generated.h"

class UApexSkillDefinition;
class UApexAbilitySystemComponent;

// ============================================================================
// FApexAbilitySetAbilityGrant — 单个能力的授予条目
// ============================================================================

USTRUCT(BlueprintType)
struct FApexAbilitySetAbilityGrant
{
	GENERATED_BODY()

	/** 技能定义（必填；强类型 TObjectPtr 本身即正确过滤） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UApexSkillDefinition> SkillDefinition;

	/** 授予时的技能等级 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	/** 输入绑定 Tag（可空；有效时必须属于 InputTag.Ability 具体子 Tag） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag.Ability"))
	FGameplayTag InputTag;
};

// ============================================================================
// FApexAbilitySetGrantedHandles — 授予产生的运行时 Handle
// ============================================================================

USTRUCT()
struct FApexAbilitySetGrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(FGameplayAbilitySpecHandle Handle);
	void TakeFromAbilitySystem(UApexAbilitySystemComponent* ASC);
	bool IsEmpty() const;

private:
	/** 本次授予产生的 AbilitySpec Handle 列表 */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

// ============================================================================
// UApexAbilitySet — 能力授予清单
// ============================================================================

/**
 * UApexAbilitySet — 面向 ASC 的只读能力授予清单。
 *
 * 设计原则：
 * - 继承 UPrimaryDataAsset，由 Character Blueprint 直接引用。
 * - 不保存运行时状态（Handle 由 FApexAbilitySetGrantedHandles 持有）。
 * - GiveToAbilitySystem() 只在权威端执行；幂等性由调用方先撤销旧 Handle 再重新授予实现。
 *
 * 【Handle 回收原因】
 * 角色 Possessed→UnPossessed 或死亡重生时，必须通过 TakeFromAbilitySystem
 * 清除上次授予的 Handle，避免重复 Spec 或幽灵能力。
 */
UCLASS(BlueprintType, Const)
class APEX_API UApexAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 授予条目列表 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Set", meta = (TitleProperty = "SkillDefinition"))
	TArray<FApexAbilitySetAbilityGrant> GrantedAbilities;

	/**
	 * 授予此 AbilitySet 中的所有能力到指定 ASC。
	 * 只在权威端执行。
	 * @param ASC 目标 ASC
	 * @param OutGrantedHandles 输出本次授予的 Handle（调用方负责回收）
	 */
	void GiveToAbilitySystem(UApexAbilitySystemComponent* ASC, FApexAbilitySetGrantedHandles* OutGrantedHandles) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
