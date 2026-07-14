# 当前代码设计预案

生成日期：2026-07-14
维护规则：这里记录实施前已经讨论并确认的大类、关键变量、关键函数、Tag、属性或配置结构。子代理实施后新增的、偏离预案的、需要再次确认的内容，写入 `Agent/Reviews/`，不要回填到这里伪装成事前设计。

## 当前设计主题

Character 冷启动基础类。

## 设计边界

本阶段只处理：

- 角色基础身体类。
- 玩家角色相机和输入。
- 基础移动与视角。

本阶段不处理：

- AnimInstance。
- Montage 播放。
- GAS / ASC / AttributeSet。
- GameplayTag。
- 技能输入。
- DataAsset。
- 蓝图资产自动创建。

## 当前 C++ 文件命名

| 文件 | 状态 | 说明 |
| --- | --- | --- |
| `Source/Apex/Public/Character/ApexCharacterBase.h` | 已确认 | 基础角色公共头文件。 |
| `Source/Apex/Private/Character/ApexCharacterBase.cpp` | 已确认 | 基础角色实现。 |
| `Source/Apex/Public/Character/ApexPlayerCharacter.h` | 已确认 | 玩家角色公共头文件。 |
| `Source/Apex/Private/Character/ApexPlayerCharacter.cpp` | 已确认 | 玩家角色实现。 |

说明：用户已确认类名和文件名；最终目录采用 UE 模块长期规范，公共头文件放 `Public`，实现文件放 `Private`。`CameraSpringArm` 改名后 `ApexEditor Win64 Development` 编译通过。

## 已确认 C++ 类设计

| 类名 | 父类 | 职责 | 当前状态 |
| --- | --- | --- | --- |
| `AApexCharacterBase` | `ACharacter` | 未来所有战斗角色的基础身体类，配置 Capsule / Mesh / CharacterMovement 默认值。 | 已确认。 |
| `AApexPlayerCharacter` | `AApexCharacterBase` | 玩家可控角色，拥有 Camera / SpringArm，绑定 Move / Look / Jump。 | 已确认。 |

## 分层理由

`AApexCharacterBase` 不放 Camera 和 Input，因为敌人、召唤物、人形 NPC、训练靶等未来也可能继承它。把玩家输入和摄像机放进 Base，会污染非玩家角色。

`AApexPlayerCharacter` 专门承载本地玩家体验，包括第三人称相机和 Enhanced Input 绑定。

## 当前建议成员变量

| 成员变量 | 所属类 | 类型 | 作用 | 审阅状态 |
| --- | --- | --- | --- | --- |
| `CameraSpringArm` | `AApexPlayerCharacter` | `TObjectPtr<USpringArmComponent>` | 第三人称摄像机臂。 | 已确认。 |
| `FollowCamera` | `AApexPlayerCharacter` | `TObjectPtr<UCameraComponent>` | 跟随摄像机。 | 已确认。 |
| `JumpAction` | `AApexPlayerCharacter` | `TObjectPtr<UInputAction>` | 跳跃输入。 | 已确认。 |
| `MoveAction` | `AApexPlayerCharacter` | `TObjectPtr<UInputAction>` | 移动输入，Axis2D。 | 已确认。 |
| `LookAction` | `AApexPlayerCharacter` | `TObjectPtr<UInputAction>` | 手柄或通用视角输入。 | 已确认。 |
| `MouseLookAction` | `AApexPlayerCharacter` | `TObjectPtr<UInputAction>` | 鼠标视角输入。 | 已确认。 |

## 当前建议函数

| 函数 | 所属类 | 作用 | 审阅状态 |
| --- | --- | --- | --- |
| `SetupPlayerInputComponent` | `AApexPlayerCharacter` | 绑定 Enhanced Input。 | 已确认。 |
| `Move` | `AApexPlayerCharacter` | 接收 `FInputActionValue` 并转发到 `DoMove`。 | 已确认。 |
| `Look` | `AApexPlayerCharacter` | 接收 `FInputActionValue` 并转发到 `DoLook`。 | 已确认。 |
| `DoMove` | `AApexPlayerCharacter` | 按控制器 yaw 计算前后左右方向并调用 `AddMovementInput`。 | 已确认。 |
| `DoLook` | `AApexPlayerCharacter` | 调用 `AddControllerYawInput` / `AddControllerPitchInput`。 | 已确认。 |
| `DoJumpStart` | `AApexPlayerCharacter` | 调用 `Jump`。 | 已确认。 |
| `DoJumpEnd` | `AApexPlayerCharacter` | 调用 `StopJumping`。 | 已确认。 |

## 后续可能新增但本阶段不做

| 名称 | 类型 | 说明 |
| --- | --- | --- |
| `UApexCharacterAnimInstance` | `UAnimInstance` 子类 | 后续动画蓝图数据基类；当前不做。 |
| `UApexCharacterDefinition` | DataAsset | 后续沉淀 Mesh / AnimClass / 移动参数；当前不做。 |
| `AApexEnemyCharacter` | Character 子类 | 后续敌人基础类；当前不做。 |
| `AApexSkillRuntimeAbility` | GameplayAbility 子类 | 后续 GAS 入口；当前不做。 |
