// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ApexCharacterBase.h"
#include "ApexPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

/**
 * AApexPlayerCharacter — 玩家可控制角色。
 *
 * 设计原则：
 * - 从 AApexCharacterBase 派生，继承基础身体配置。
 * - 拥有 SpringArm 和 Camera，提供第三人称视角。
 * - 绑定 Enhanced Input 的 Move / Look / Jump 输入 Action。
 * - 不重复添加 MappingContext（由 AApexPlayerController 统一管理），避免重复输入上下文。
 * - 不包含技能输入、GAS、Montage 播放。
 *
 * 后续可在蓝图中设置 Mesh 为 Paragon 英雄资产、配置 InputAction 引用。
 */
UCLASS(Abstract)
class APEX_API AApexPlayerCharacter : public AApexCharacterBase
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

public:
	/** Constructor */
	AApexPlayerCharacter();

protected:
	// --- 输入绑定 ---

	/** 绑定 Enhanced Input Action 到处理函数 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// --- 输入回调（由 Enhanced Input 触发）---

	/** MoveAction / MouseLookAction 回调，提取 FVector2D 后路由到 DoMove / DoLook */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

public:
	// --- 蓝图可调用的输入处理 ---

	/** 处理移动输入：Right=左右(X), Forward=前后(Y) */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** 处理视角输入：Yaw=水平(X), Pitch=垂直(Y) */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** 跳跃按下 */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** 跳跃松开 */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:
	/** 获取 CameraSpringArm */
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }

	/** 获取 FollowCamera */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
