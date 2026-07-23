// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayTags/ApexGameplayTags.h"

namespace ApexGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability, "InputTag.Ability", "Ability 输入根标签，仅用于分类。");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_BasicAttack, "InputTag.Ability.BasicAttack", "普通攻击槽。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Skill1, "InputTag.Ability.Skill1", "第一个小技能，默认 Q。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Skill2, "InputTag.Ability.Skill2", "第二个小技能，默认 E。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Ultimate, "InputTag.Ability.Ultimate", "大招槽，默认 X。");
}
