// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApexCharacterBase.generated.h"

/**
 * AApexCharacterBase — Apex 项目所有战斗角色的基础身体类。
 *
 * 设计原则：
 * - 只负责 Capsule / Mesh / CharacterMovement 的默认配置。
 * - 不包含 Camera / SpringArm，因为这些属于玩家视角层，不应强加给 AI、敌人、召唤物。
 * - 不包含输入绑定，输入只属于玩家可控制角色。
 * - 不包含 GAS / Ability / Montage，这些将在后续派生类或组件中添加。
 *
 * 派生关系：
 * - AApexPlayerCharacter（玩家）从本类派生。
 * - 未来 AEnemyCharacterBase / ASummonBase 等也从本类派生。
 */
UCLASS(Abstract)
class APEX_API AApexCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/** Constructor */
	AApexCharacterBase();

protected:
	// 基础移动默认值在构造函数中配置，此处不添加额外成员。
	// 后续可在此处添加战斗角色共有属性（如 MaxHealth、TeamID 等）。
};
