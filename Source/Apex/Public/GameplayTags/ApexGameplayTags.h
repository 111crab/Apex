// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// 只读表移动等常规输入仍使用 InputAction + TriggerEvent；Ability 输入槽统一使用 InputTag 路由。
namespace ApexGameplayTags
{
	// 根标签，仅用于分类和验证。
	APEX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability);

	APEX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_BasicAttack);
	APEX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Skill1);
	APEX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Skill2);
	APEX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Ultimate);
}
