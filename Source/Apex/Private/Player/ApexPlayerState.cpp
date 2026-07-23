// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/ApexPlayerState.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ApexVitalAttributeSet.h"

AApexPlayerState::AApexPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UApexAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// ASC 需要通过网络复制到客户端。
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed：GE 完整信息复制给拥有者，Tag/Cue 最小信息复制给其他客户端。
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 网络更新频率（每秒）。
	SetNetUpdateFrequency(100.f);

	// Vital AttributeSet 与 ASC 都由 PlayerState 持有；ASC 初始化时会发现并注册该默认子对象。
	VitalAttributeSet = CreateDefaultSubobject<UApexVitalAttributeSet>(TEXT("VitalAttributeSet"));
}

UAbilitySystemComponent* AApexPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UApexAbilitySystemComponent* AApexPlayerState::GetApexAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const UApexVitalAttributeSet* AApexPlayerState::GetVitalAttributeSet() const
{
	return VitalAttributeSet;
}
