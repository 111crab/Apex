// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/Abilities/ApexGameplayAbility.h"
#include "ApexSkillDefinition.generated.h"

/**
 * UApexSkillDefinition — 单个技能的根配置资产。
 *
 * 设计原则：
 * - 继承 UPrimaryDataAsset，作为技能身份入口。
 * - 本批只确定"使用哪个 GA 模板"这一稳定关系。
 * - Identity、Presentation、目标规则、CombatEntity、Effect/Cue 后续增量加入。
 *
 * 【为什么 SourceObject 指向 SkillDefinition 而非 GA Class？】
 * 同一个 UApexGameplayAbility 模板可被多个技能复用（如多个投射物技能）。
 * SourceObject 指向 SkillDefinition 让 GA 在运行时取回具体技能的身份和参数。
 */
UCLASS(BlueprintType, Const)
class APEX_API UApexSkillDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 该技能对应的 GA 模板类 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TSubclassOf<UApexGameplayAbility> AbilityTemplateClass;

	/** 返回 GA 模板类 */
	TSubclassOf<UApexGameplayAbility> GetAbilityTemplateClass() const { return AbilityTemplateClass; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
