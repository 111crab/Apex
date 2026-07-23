// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ApexGameplayAbility.generated.h"

class UApexAbilitySystemComponent;
class AApexPlayerCharacter;
class UApexSkillDefinition;

UENUM(BlueprintType)
enum class EApexAbilityActivationPolicy : uint8
{
	/** 按下时尝试激活；引导/蓄力能力激活后仍可接收 Released。 */
	OnInputTriggered UMETA(DisplayName = "OnInputTriggered"),

	/** 按住期间，只要能力未激活就持续尝试。 */
	WhileInputActive UMETA(DisplayName = "WhileInputActive")
};

/**
 * UApexGameplayAbility — 所有 Apex GameplayAbility 的根基类。
 *
 * 设计原则：
 * - 继承 UGameplayAbility，提供 EApexAbilityActivationPolicy 激活策略。
 * - 提供类型安全的 ASC 和 Character 访问方法。
 * - GetSkillDefinition() 从 AbilitySpec::SourceObject 取回 SkillDefinition。
 * - 不在此处实现具体技能逻辑；未来 ProjectileCast 等模板继承此类。
 *
 * 【为什么 InstancedPerActor？】
 * 同一 Pawn 上的多个技能可能使用不同的 SkillDefinition 但共享相同的 GA 模板类。
 * InstancedPerActor 确保每个授予的 Spec 都有独立的 GA 实例，互不干扰。
 *
 * 【为什么 NetExecutionPolicy 为 LocalPredicted？】
 * 本地玩家技能输入需要即时反馈。GAS 预测机制允许客户端先执行，
 * 服务端确认后回滚或补发修正。
 */
UCLASS(Abstract, Blueprintable)
class APEX_API UApexGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UApexGameplayAbility();

	/** 激活策略 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apex|Ability")
	EApexAbilityActivationPolicy ActivationPolicy = EApexAbilityActivationPolicy::OnInputTriggered;

	/** 返回激活策略 */
	EApexAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	/** 返回类型安全的 Apex ASC */
	UFUNCTION(BlueprintPure, Category = "Apex|Ability")
	UApexAbilitySystemComponent* GetApexAbilitySystemComponentFromActorInfo() const;

	/** 返回类型安全的 Apex Player Character */
	UFUNCTION(BlueprintPure, Category = "Apex|Ability")
	AApexPlayerCharacter* GetApexPlayerCharacterFromActorInfo() const;

	/**
	 * 从当前 AbilitySpec 的 SourceObject 取回 SkillDefinition。
	 * SourceObject 在 AbilitySet 授予时被设置为对应的 UApexSkillDefinition。
	 */
	UFUNCTION(BlueprintPure, Category = "Apex|Ability")
	const UApexSkillDefinition* GetSkillDefinition() const;
};
