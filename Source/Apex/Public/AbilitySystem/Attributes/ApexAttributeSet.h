// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ApexAttributeSet.generated.h"

class UApexAbilitySystemComponent;

// 统一生成 Get/Set/Init 四个函数的通用宏。
// 每个子类在声明 FGameplayAttributeData 成员后调用此宏。
#define APEX_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * UApexAttributeSet — Apex 所有 AttributeSet 的公共基类。
 *
 * 设计原则：
 * - 不保存具体业务属性（如 Health、Damage 等），只为子类提供项目约定。
 * - 提供统一的 ATTRIBUTE_ACCESSORS 宏和类型安全的 ASC 访问方法。
 * - 未来可在此处添加项目级属性事件类型。
 *
 * 【为什么基类不保存具体属性？】
 * 属性职责应随不变量拆分：Health 属于 VitalAttributeSet，
 * 攻击力/护甲属于 CombatAttributeSet。基类混入具体属性
 * 会导致职责模糊，且无法按语义选择复制条件和结算逻辑。
 */
UCLASS()
class APEX_API UApexAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** 返回类型安全的 Apex ASC */
	UApexAbilitySystemComponent* GetApexAbilitySystemComponent() const;
};
