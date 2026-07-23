// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ApexCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/ApexAbilitySet.h"
#include "ApexPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UApexAbilitySet;
class UApexAbilityInputConfig;
struct FInputActionValue;

/**
 * AApexPlayerCharacter — 玩家可控制角色。
 *
 * 设计原则：
 * - 从 AApexCharacterBase 派生，继承基础身体配置。
 * - 拥有 SpringArm 和 Camera，提供第三人称视角。
 * - 绑定 Enhanced Input 的 Move / Look / Jump 输入 Action。
 * - 不重复添加 MappingContext（由 AApexPlayerController 统一管理）。
 *
 * 【Ability 授予】：
 * - CoreAbilitySet 由 PossessedBy 在服务器授予。
 * - 客户端不执行授予——AbilitySpec 通过 GAS 复制到达。
 * - UnPossessed/EndPlay 时幂等撤销 Handle。
 *
 * 【Ability 输入路由】：
 * - SetupPlayerInputComponent 遍历 AbilityInputConfig 绑定 Ability InputAction。
 * - 输入回调调用 InputAbilityTagPressed/Released，转发到 ASC 队列。
 */
UCLASS(Abstract)
class APEX_API AApexPlayerCharacter : public AApexCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	// --- 组件 ---

	/** 相机摇臂，控制相机与角色的距离和碰撞检测 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraSpringArm;

	/** 跟随相机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// --- 输入 Action 引用（由蓝图配置）---

	/** 跳跃输入 Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> JumpAction;

	/** 移动输入 Action（WASD / 左摇杆） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> MoveAction;

	/** 手柄视角输入 Action（右摇杆） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> LookAction;

	/** 鼠标视角输入 Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> MouseLookAction;

	// --- Ability 资产配置 ---

	/** 核心能力集 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UApexAbilitySet> CoreAbilitySet;

	/** Ability 输入绑定配置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UApexAbilityInputConfig> AbilityInputConfig;

public:
	/** Constructor */
	AApexPlayerCharacter();

	// --- IAbilitySystemInterface ---
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// --- 输入绑定 ---

	/** 绑定 Enhanced Input Action 到处理函数 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// --- 输入回调（由 Enhanced Input 触发）---

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

public:
	// --- 蓝图可调用的输入处理 ---

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// --- GAS 初始化 ---

	void InitializeAbilitySystemActorInfo();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// --- Ability 授予 ---

	/** 服务器 PossessedBy 时授予 CoreAbilitySet */
	void GrantCoreAbilitySet();

	/** 服务器 UnPossessed/EndPlay 时撤销 CoreAbilitySet */
	void RemoveCoreAbilitySet();

	/** 通知 ASC 有 Ability InputTag 按下 */
	void InputAbilityTagPressed(FGameplayTag InputTag);

	/** 通知 ASC 有 Ability InputTag 释放 */
	void InputAbilityTagReleased(FGameplayTag InputTag);

	virtual void UnPossessed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** 本次授予产生的 Handle（用于后续撤销） */
	UPROPERTY()
	FApexAbilitySetGrantedHandles CoreAbilitySetHandles;
};
