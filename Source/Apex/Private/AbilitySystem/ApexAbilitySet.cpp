// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/ApexAbilitySet.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "AbilitySystem/Data/ApexSkillDefinition.h"
#include "AbilitySystem/Abilities/ApexGameplayAbility.h"
#include "Apex.h"
#include "Misc/DataValidation.h"
#include "GameplayTags/ApexGameplayTags.h"

// ============================================================================
// FApexAbilitySetGrantedHandles
// ============================================================================

void FApexAbilitySetGrantedHandles::AddAbilitySpecHandle(FGameplayAbilitySpecHandle Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FApexAbilitySetGrantedHandles::TakeFromAbilitySystem(UApexAbilitySystemComponent* ASC)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
	AbilitySpecHandles.Empty();
}

bool FApexAbilitySetGrantedHandles::IsEmpty() const
{
	return AbilitySpecHandles.IsEmpty();
}

// ============================================================================
// UApexAbilitySet
// ============================================================================

void UApexAbilitySet::GiveToAbilitySystem(
	UApexAbilitySystemComponent* ASC,
	FApexAbilitySetGrantedHandles* OutGrantedHandles) const
{
	if (!ASC || !OutGrantedHandles)
	{
		return;
	}

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FApexAbilitySetAbilityGrant& Grant : GrantedAbilities)
	{
		if (!Grant.SkillDefinition)
		{
			UE_LOG(LogApex, Warning, TEXT("[UApexAbilitySet] 跳过空 SkillDefinition 的授予条目。"));
			continue;
		}

		const TSubclassOf<UApexGameplayAbility> AbilityClass = Grant.SkillDefinition->GetAbilityTemplateClass();
		if (!AbilityClass)
		{
			UE_LOG(LogApex, Warning, TEXT("[UApexAbilitySet] SkillDefinition [%s] 的 AbilityTemplateClass 为空，跳过。"),
				*GetNameSafe(Grant.SkillDefinition));
			continue;
		}

		if (Grant.AbilityLevel <= 0)
		{
			UE_LOG(LogApex, Warning, TEXT("[UApexAbilitySet] AbilityLevel 必须 > 0，当前为 %d，跳过。"), Grant.AbilityLevel);
			continue;
		}

		// InputTag 若有效必须属于 InputTag.Ability 的具体子 Tag。
		if (Grant.InputTag.IsValid())
		{
			if (!Grant.InputTag.MatchesTag(ApexGameplayTags::InputTag_Ability) ||
				Grant.InputTag.MatchesTagExact(ApexGameplayTags::InputTag_Ability))
			{
				UE_LOG(LogApex, Warning, TEXT("[UApexAbilitySet] InputTag [%s] 不是 InputTag.Ability 的具体子 Tag，跳过该授予条目。"),
					*Grant.InputTag.ToString());
				continue;
			}
		}

		FGameplayAbilitySpec Spec(AbilityClass, Grant.AbilityLevel);
		Spec.SourceObject = Grant.SkillDefinition;

		if (Grant.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		}

		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		OutGrantedHandles->AddAbilitySpecHandle(Handle);
	}
}

#if WITH_EDITOR
EDataValidationResult UApexAbilitySet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<FGameplayTag> InputTagSet;
	for (const FApexAbilitySetAbilityGrant& Grant : GrantedAbilities)
	{
		if (!Grant.SkillDefinition)
		{
			Context.AddError(FText::FromString("GrantedAbilities 中存在空 SkillDefinition。"));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (Grant.AbilityLevel <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("GrantedAbilities 中 SkillDefinition [%s] 的 AbilityLevel 必须 > 0。"), *GetNameSafe(Grant.SkillDefinition))));
			Result = EDataValidationResult::Invalid;
		}

		if (Grant.InputTag.IsValid())
		{
			// InputTag 必须属于 InputTag.Ability 的具体子 Tag。
			if (!Grant.InputTag.MatchesTag(ApexGameplayTags::InputTag_Ability) ||
				Grant.InputTag.MatchesTagExact(ApexGameplayTags::InputTag_Ability))
			{
				Context.AddError(FText::FromString(FString::Printf(
					TEXT("InputTag [%s] 不是 InputTag.Ability 的具体子 Tag。"), *Grant.InputTag.ToString())));
				Result = EDataValidationResult::Invalid;
			}

			if (InputTagSet.Contains(Grant.InputTag))
			{
				Context.AddError(FText::FromString(FString::Printf(
					TEXT("InputTag [%s] 在同一 AbilitySet 中重复。"), *Grant.InputTag.ToString())));
				Result = EDataValidationResult::Invalid;
			}
			InputTagSet.Add(Grant.InputTag);
		}
	}

	return Result;
}
#endif
