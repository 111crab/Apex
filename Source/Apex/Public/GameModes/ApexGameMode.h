// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ApexGameMode.generated.h"

/**
 * AApexGameMode — Apex 服务端游戏规则基类。
 * 当前为空壳，只作为蓝图父类使用。游戏规则与 GAS 初始化不在此处硬编码。
 */
UCLASS(abstract)
class APEX_API AApexGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AApexGameMode();
};



