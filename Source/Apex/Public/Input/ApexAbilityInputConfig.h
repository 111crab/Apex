// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ApexAbilityInputConfig.generated.h"

class UInputAction;

// ============================================================================
// FApexAbilityInputAction — 单条 Ability 输入绑定
// ============================================================================

USTRUCT(BlueprintType)
struct FApexAbilityInputAction
{
	GENERATED_BODY()

	/** 增强输入 Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction;

	/** 对应的 Ability 输入 Tag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag.Ability"))
	FGameplayTag InputTag;
};

// ============================================================================
// UApexAbilityInputConfig — Ability 输入配置
// ============================================================================

/**
 * UApexAbilityInputConfig — 将 Enhanced Input Action 映射到 InputTag。
 *
 * 设计原则：
 * - 继承 UDataAsset，由 Character Blueprint 直接引用。
 * - SetupPlayerInputComponent 遍历此配置绑定 Ability InputAction。
 * - 只处理 Ability 输入；Jump/Move/Look 保持原样。
 */
UCLASS(BlueprintType, Const)
class APEX_API UApexAbilityInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Ability 输入绑定列表 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Input", meta = (TitleProperty = "InputTag"))
	TArray<FApexAbilityInputAction> AbilityInputActions;

	/** 返回绑定列表 */
	const TArray<FApexAbilityInputAction>& GetAbilityInputActions() const { return AbilityInputActions; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
