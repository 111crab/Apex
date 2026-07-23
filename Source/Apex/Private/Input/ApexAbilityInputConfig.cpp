// Copyright Epic Games, Inc. All Rights Reserved.

#include "Input/ApexAbilityInputConfig.h"
#include "InputAction.h"
#include "Misc/DataValidation.h"
#include "GameplayTags/ApexGameplayTags.h"

#if WITH_EDITOR
EDataValidationResult UApexAbilityInputConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<const UInputAction*> ActionSet;
	TSet<FGameplayTag> TagSet;

	for (const FApexAbilityInputAction& Action : AbilityInputActions)
	{
		if (!Action.InputAction)
		{
			Context.AddError(FText::FromString("AbilityInputActions 中存在空 InputAction。"));
			Result = EDataValidationResult::Invalid;
			continue;
		}
		if (!Action.InputTag.IsValid())
		{
			Context.AddError(FText::FromString("AbilityInputActions 中存在无效 InputTag。"));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		// InputTag 必须属于 InputTag.Ability 的具体子 Tag。
		if (!Action.InputTag.MatchesTag(ApexGameplayTags::InputTag_Ability) ||
			Action.InputTag.MatchesTagExact(ApexGameplayTags::InputTag_Ability))
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("InputTag [%s] 不是 InputTag.Ability 的具体子 Tag。"), *Action.InputTag.ToString())));
			Result = EDataValidationResult::Invalid;
		}

		if (ActionSet.Contains(Action.InputAction))
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("InputAction [%s] 重复。"), *Action.InputAction->GetName())));
			Result = EDataValidationResult::Invalid;
		}
		ActionSet.Add(Action.InputAction);

		if (TagSet.Contains(Action.InputTag))
		{
			Context.AddError(FText::FromString(FString::Printf(
				TEXT("InputTag [%s] 重复。"), *Action.InputTag.ToString())));
			Result = EDataValidationResult::Invalid;
		}
		TagSet.Add(Action.InputTag);
	}

	return Result;
}
#endif
