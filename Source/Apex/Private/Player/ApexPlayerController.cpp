// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/ApexPlayerController.h"
#include "Player/ApexPlayerState.h"
#include "AbilitySystem/ApexAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Apex.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AApexPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogApex, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AApexPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AApexPlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

// ============================================================================
// Ability 输入处理
// ============================================================================

void AApexPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	// 仅本地 Controller 才处理 Ability 输入队列。
	// 远端 Pawn 的输入由服务端驱动，不应在此处理。
	if (IsLocalController())
	{
		UApexAbilitySystemComponent* ASC = GetApexAbilitySystemComponent();
		if (ASC)
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
}

UApexAbilitySystemComponent* AApexPlayerController::GetApexAbilitySystemComponent() const
{
	AApexPlayerState* ApexPS = GetPlayerState<AApexPlayerState>();
	return ApexPS ? ApexPS->GetApexAbilitySystemComponent() : nullptr;
}
