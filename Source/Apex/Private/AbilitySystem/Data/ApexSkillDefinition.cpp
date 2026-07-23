// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Data/ApexSkillDefinition.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UApexSkillDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!AbilityTemplateClass)
	{
		Context.AddError(FText::FromString("AbilityTemplateClass 不能为空。"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
