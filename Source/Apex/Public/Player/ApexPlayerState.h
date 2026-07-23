// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ApexPlayerState.generated.h"

class UApexAbilitySystemComponent;
class UApexVitalAttributeSet;

/**
 * AApexPlayerState — Apex 玩家状态。
 *
 * 设计原则：
 * - 持有 UApexAbilitySystemComponent（Owner），所有技能权限由此出。
 * - 实现 IAbilitySystemInterface，供外部系统查询 ASC。
 * - 复制模式 Mixed：GameplayEffect 完整信息复制给拥有者；
 *   GameplayTag 和 GameplayCue 等最小信息复制给其他客户端。
 * - 构造时创建 ASC，设置复制和网络更新频率。
 *
 * 【Owner / Avatar 的职责】：
 * - Owner = AApexPlayerState（权限来源，持久化）。
 * - Avatar = AApexPlayerCharacter（表现载体，可随复活/换人更换）。
 */
UCLASS()
class APEX_API AApexPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AApexPlayerState();

	// --- IAbilitySystemInterface ---
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** 返回类型安全的 UApexAbilitySystemComponent */
	UApexAbilitySystemComponent* GetApexAbilitySystemComponent() const;

	/** 返回 Vital AttributeSet（只读） */
	const UApexVitalAttributeSet* GetVitalAttributeSet() const;

private:
	/** 本玩家持有的 ASC */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ability System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UApexAbilitySystemComponent> AbilitySystemComponent;

	/** Vital 属性集（Health, MaxHealth, Mana, MaxMana） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ability System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UApexVitalAttributeSet> VitalAttributeSet;
};
