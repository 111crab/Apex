// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Attributes/ApexAttributeSet.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"

UApexAbilitySystemComponent* UApexAttributeSet::GetApexAbilitySystemComponent() const
{
	return Cast<UApexAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
