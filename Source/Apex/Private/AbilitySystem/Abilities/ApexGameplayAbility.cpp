// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/ApexGameplayAbility.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "AbilitySystem/Data/ApexSkillDefinition.h"
#include "Character/ApexPlayerCharacter.h"

UApexGameplayAbility::UApexGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

UApexAbilitySystemComponent* UApexGameplayAbility::GetApexAbilitySystemComponentFromActorInfo() const
{
	return Cast<UApexAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

AApexPlayerCharacter* UApexGameplayAbility::GetApexPlayerCharacterFromActorInfo() const
{
	return Cast<AApexPlayerCharacter>(GetAvatarActorFromActorInfo());
}

const UApexSkillDefinition* UApexGameplayAbility::GetSkillDefinition() const
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec ? Cast<UApexSkillDefinition>(Spec->SourceObject) : nullptr;
}
