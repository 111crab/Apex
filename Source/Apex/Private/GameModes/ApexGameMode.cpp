// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModes/ApexGameMode.h"
#include "Player/ApexPlayerState.h"

AApexGameMode::AApexGameMode()
{
	// 默认使用 AApexPlayerState 作为玩家状态。
	PlayerStateClass = AApexPlayerState::StaticClass();
}
