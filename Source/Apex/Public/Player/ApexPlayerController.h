// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ApexPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UApexAbilitySystemComponent;

/**
 * AApexPlayerController — Apex 本地玩家输入与 UI 入口基类。
 * 负责管理 Enhanced Input Mapping Context 与现有触屏控制入口。
 * 战斗状态、属性与技能执行不放在此处。
 *
 * 【Ability 输入处理】：
 * 重写 PostProcessInput，在每帧推进 ASC 输入队列。
 * Controller 只推进队列，不决定具体技能。
 */
UCLASS(abstract)
class APEX_API AApexPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

public:
	// --- Ability 输入处理 ---

	/** 每帧推进 ASC 输入队列 */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	/** 返回类型安全的 Apex ASC（从 PlayerState 获取） */
	UApexAbilitySystemComponent* GetApexAbilitySystemComponent() const;
};
