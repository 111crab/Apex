# 当前 UE 编辑器人工操作清单

生成日期：2026-07-16

维护规则：本文只记录当前这一轮需要用户在 Rider 和 UE 编辑器中完成的操作。进入新的人工操作阶段时由 Codex 覆盖。

## 当前目标

完成 Phase 的 Apex 自有最小动画基线：

- Rider 能识别新的 `UApexAnimInstance` C++ 文件。
- 基础输入全部切换到项目自有 IA 和 `IMC_Apex_BaseMove`。
- 创建 `BS_Apex_Phase_Locomotion` 和 `ABP_Apex_Phase`。
- 验证 Idle、Jog、主动跳跃、走下平台、下落和落地。
- 在 AnimGraph 中预留 `UpperBody` 与 `FullBody` Montage Slot。

本轮不接入 GAS、技能 Montage、GameplayEvent Notify、AimOffset、Foot IK 或锁定目标移动。

## 一、Rider 上线检查

项目文件已经由 Codex 成功生成：

```text
D:\UnrealProject\Apex\Apex.sln
D:\UnrealProject\Apex\Apex.slnx
```

1. 如果 Rider 已打开旧 Solution，关闭后重新打开 `D:\UnrealProject\Apex\Apex.sln`，或使用 Rider 的 Reload Project/Solution。
2. 确认能够看到：

```text
Source/Apex/Public/Animation/ApexAnimInstance.h
Source/Apex/Private/Animation/ApexAnimInstance.cpp
```

3. 确认类名为 `UApexAnimInstance`，父类为 `UAnimInstance`。
4. 使用 `ApexEditor | Win64 | Development Editor` 构建配置。ClaudeCode 已完成一次成功编译；这里用于确认 Rider 使用的是刚生成的项目文件。
5. 构建成功后再启动 UE 编辑器。

如果 Rider 仍显示 `Automation_Apex`，说明打开的是旧 Solution。关闭它并明确打开 `D:\UnrealProject\Apex\Apex.sln`。

## 二、禁止修改的第三方资产

本轮只读取以下 Paragon Phase 资产，不移动、不重命名、不保存修改：

```text
/Game/ParagonPhase/Characters/Heroes/Phase/Meshes/phase_Skeleton
/Game/ParagonPhase/Characters/Heroes/Phase/Animations/Idle
/Game/ParagonPhase/Characters/Heroes/Phase/Animations/Jog_Fwd
/Game/ParagonPhase/Characters/Heroes/Phase/Animations/Jump_Start
/Game/ParagonPhase/Characters/Heroes/Phase/Animations/Jump_Apex
/Game/ParagonPhase/Characters/Heroes/Phase/Animations/Jump_Land
```

新建资产统一放入：

```text
/Game/Blueprints/
```

## 三、确认项目自有 InputAction

打开 `/Game/Blueprints/Input/Actions/`，确认四个资产存在并检查 `Value Type`：

| 资产 | Value Type |
| --- | --- |
| `IA_Jump` | Digital / Bool |
| `IA_Move` | Axis2D |
| `IA_Look` | Axis2D |
| `IA_MouseLook` | Axis2D |

如果类型不一致，先修改并保存。不要重新创建第二套同名资产。

## 四、创建 IMC_Apex_BaseMove

### 1. 创建资产

1. 打开 `/Game/Blueprints/Input/`。
2. 右键空白处，选择 `Input -> Input Mapping Context`。
3. 命名为 `IMC_Apex_BaseMove`。
4. 打开该资产，在 `Mappings` 中添加以下映射。

### 2. 跳跃

| InputAction | Key | Modifiers |
| --- | --- | --- |
| `IA_Jump` | `Space Bar` | 无 |
| `IA_Jump` | `Gamepad Face Button Bottom` | 无 |

### 3. 键盘移动

`IA_Move` 是 `Axis2D`，C++ 约定 `X=Right`、`Y=Forward`：

| InputAction | Key | Modifiers 与顺序 | 输出 |
| --- | --- | --- | --- |
| `IA_Move` | `W` | `Swizzle Input Axis Values`，Order=`YXZ` | Y=+1 |
| `IA_Move` | `S` | `Negate`，然后 `Swizzle Input Axis Values`，Order=`YXZ` | Y=-1 |
| `IA_Move` | `A` | `Negate` | X=-1 |
| `IA_Move` | `D` | 无 | X=+1 |
| `IA_Move` | `Gamepad Left Thumbstick 2D-Axis` | 无 | X/Y |

注意：`W/S` 必须使用 `Swizzle` 把键盘按键默认的 X 轴值转换到 Y 轴。否则 W/S 会被识别成左右移动。

### 4. 视角

| InputAction | Key | Modifiers |
| --- | --- | --- |
| `IA_MouseLook` | `Mouse XY 2D-Axis` | `Negate`，只启用 Y，X/Z 关闭 |
| `IA_Look` | `Gamepad Right Thumbstick 2D-Axis` | 无 |

当前 Phase 基线已验证鼠标垂直方向需要取反。只修改 `IA_MouseLook -> Mouse XY` 这一条映射，不修改 C++ `DoLook()`，也不要反转 X。

保存 `IMC_Apex_BaseMove`。

## 五、切换角色和 PlayerController 输入引用

### 1. BP_Hero_Phase

打开 `/Game/Blueprints/Characters/BP_Hero_Phase`，进入 `Class Defaults -> Input`：

| 属性 | 设置为 |
| --- | --- |
| `JumpAction` | `/Game/Blueprints/Input/Actions/IA_Jump` |
| `MoveAction` | `/Game/Blueprints/Input/Actions/IA_Move` |
| `LookAction` | `/Game/Blueprints/Input/Actions/IA_Look` |
| `MouseLookAction` | `/Game/Blueprints/Input/Actions/IA_MouseLook` |

编译并保存。

### 2. BP_ThirdPersonPlayerController

打开 `/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController`，进入 `Class Defaults -> Input -> Input Mappings`：

| 属性 | 设置 |
| --- | --- |
| `Default Mapping Contexts` | 只保留一个元素：`IMC_Apex_BaseMove` |
| `Mobile Excluded Mapping Contexts` | 清空 |
| `Force Touch Controls` | `false` |

不要在 Character 或关卡蓝图中再次调用 `Add Mapping Context`。当前 `AApexPlayerController` 会为本地玩家统一添加这里配置的 IMC。

## 六、创建动画目录

在 Content Browser 中创建：

```text
/Game/Blueprints/Characters/Phase/Animation/
```

本轮创建的 `BS_Apex_Phase_Locomotion` 与 `ABP_Apex_Phase` 都放在这里。

## 七、创建 BS_Apex_Phase_Locomotion

1. 在 `/Game/Blueprints/Characters/Phase/Animation/` 右键。
2. 选择 `Animation -> Blend Space 1D`。
3. Skeleton 选择 `phase_Skeleton`。
4. 命名为 `BS_Apex_Phase_Locomotion`。
5. 打开后确认 `Axis Settings` 只有 `Horizontal Axis`。如果同时出现可配置的 `Vertical Axis`，说明误建成了二维 `Blend Space`，应删除这个新资产并重新创建 `Blend Space 1D`。
6. 打开 BlendSpace，在 Asset Details 中配置水平轴：

| 设置 | 值 |
| --- | --- |
| Axis Name | `GroundSpeed` |
| Minimum Axis Value | `0` |
| Maximum Axis Value | `500` |
| Number of Grid Divisions | `5` |
| Smoothing Time | `0.15` |

7. 在编辑器下方的 `Blend Graph` 网格中，将 Phase 的 `Idle` 放到 `0`。
8. 将 Phase 的 `Jog_Fwd` 放到 `500`。
9. 拖动绿色预览点检查：0 时为 Idle，接近 500 时为 Jog，中间平滑混合。
10. 保存资产。

这个 BlendSpace 只决定显示哪种姿势，不会改变 CharacterMovement 的真实速度。

## 八、创建 ABP_Apex_Phase

1. 在 `/Game/Blueprints/Characters/Phase/Animation/` 右键。
2. 选择 `Animation -> Animation Blueprint`。
3. Target Skeleton 选择 `phase_Skeleton`。
4. Parent Class 选择 `UApexAnimInstance`。
5. 命名为 `ABP_Apex_Phase`。

如果创建窗口没有 Parent Class 选项：

1. 先使用 `AnimInstance` 创建。
2. 打开 AnimBP，进入 `Class Settings`。
3. 将 `Parent Class` 改为 `UApexAnimInstance`。
4. Compile，确认没有父类或 Skeleton 错误。

`EventGraph` 保持为空。不要添加 `Event Blueprint Update Animation`、`Try Get Pawn Owner` 或每帧 Cast；`GroundSpeed` 等状态已经由 C++ 父类计算。

### 显示 C++ 父类变量

1. 先打开 `Class Settings`，再次确认 `Parent Class = UApexAnimInstance`；如果仍是 `AnimInstance`，继承变量不会出现。
2. Compile 一次 AnimBP。
3. 在左侧 `My Blueprint` 面板打开齿轮或 View Options，勾选 `Show Inherited Variables`。
4. 在 Variables 中展开 `Animation|Locomotion`，或搜索蓝图友好名称：

```text
Ground Speed
Vertical Speed
Has Acceleration
Is Falling
```

C++ 名称 `GroundSpeed` 在蓝图界面中通常显示为 `Ground Speed`。这些变量是 `BlueprintReadOnly`，可以作为 Get 节点读取，但不能在 AnimBP 中 Set。

## 九、创建 Locomotion 状态机

### 1. 建立状态机

1. 在 `ABP_Apex_Phase` 的 AnimGraph 中添加 `State Machine`。
2. 命名为 `Locomotion`。
3. 双击进入，创建四个状态：

```text
Grounded
JumpStart
Falling
Land
```

4. `Entry` 连接到 `Grounded`。

### 2. Grounded

1. 双击 `Grounded`。
2. 添加 `BS_Apex_Phase_Locomotion` BlendSpace Player。
3. 从 `My Blueprint` 将继承变量 `Ground Speed` 拖入图中并选择 `Get`，连接到 BlendSpace 的 `GroundSpeed` 输入。
4. 将输出连接到 State Result。

### 3. JumpStart

1. 双击 `JumpStart`。
2. 添加 `Jump_Start` Sequence Player。
3. 关闭 `Loop Animation`。
4. 连接到 State Result。

### 4. Falling

1. 双击 `Falling`。
2. 添加 `Jump_Apex` Sequence Player。
3. 关闭 `Loop Animation`；该动画播放结束后保持最后一帧，避免在较长滞空中反复播放 Apex 动作。
4. 连接到 State Result。

### 5. Land

1. 双击 `Land`。
2. 添加 `Jump_Land` Sequence Player。
3. 关闭 `Loop Animation`。
4. 连接到 State Result。

## 十、配置状态转换

在状态机中创建并配置以下转换箭头。变量来自 `UApexAnimInstance`，在蓝图中可能显示为空格友好名，例如 `Is Falling`、`Vertical Speed`。

### 1. Grounded -> JumpStart

```text
bIsFalling AND VerticalSpeed > 0
```

用途：Space 主动起跳时先播放起跳动画。

### 2. Grounded -> Falling

```text
bIsFalling AND VerticalSpeed <= 0
```

用途：角色直接走下平台时跳过 JumpStart，避免播放主动蹬地动作。

### 3. JumpStart -> Falling

```text
VerticalSpeed <= 0
OR
Get Relevant Anim Time Remaining Fraction (JumpStart) <= 0.1
```

用途：开始下降，或者起跳动画即将结束时进入空中姿势。

### 4. Falling -> Land

```text
NOT bIsFalling
```

用途：CharacterMovement 再次接触可行走地面时播放落地动画。

### 5. Land -> Grounded

```text
Get Relevant Anim Time Remaining Fraction (Land) <= 0.1
```

用途：落地动画即将结束时恢复 Idle/Jog BlendSpace。

### 6. 建议 Crossfade Duration

| 转换 | Duration |
| --- | --- |
| Grounded -> JumpStart | `0.05` |
| Grounded -> Falling | `0.10` |
| JumpStart -> Falling | `0.10` |
| Falling -> Land | `0.05` |
| Land -> Grounded | `0.10` |

这些是冷启动观察值，不是最终动画手感参数。

## 十一、搭建 AnimGraph 输出和 Montage Slot 预留

目标结构：

```text
Locomotion State Machine
-> Save Cached Pose "LocomotionPose"
-> UpperBody 分层
-> FullBody Slot
-> Output Pose
```

按以下方式连接，避免把卡槽错误地串成两个全身覆盖：

1. 将 `Locomotion` State Machine 输出连接到 `Save Cached Pose`，名称填写 `LocomotionPose`。
2. 添加第一个 `Use Cached Pose: LocomotionPose`，连接到 `Layered Blend Per Bone` 的 `Base Pose`。
3. 添加第二个 `Use Cached Pose: LocomotionPose`，连接到一个 `Slot` 节点的 Source。
4. 将这个 Slot 设置为 `UpperBody`，再把它的输出连接到 `Layered Blend Per Bone` 的 `Blend Pose 0`。
5. 在 `Layered Blend Per Bone -> Layer Setup[0] -> Branch Filters[0]` 中设置：

| 设置 | 值 |
| --- | --- |
| Bone Name | `spine_01` |
| Blend Depth | `0` |
| Blend Weight 0 | `1.0` |

6. 将 `Layered Blend Per Bone` 输出连接到第二个 Slot 节点。
7. 将第二个 Slot 设置为 `FullBody`。
8. 将 `FullBody` Slot 输出连接到 `Output Pose`。
9. Compile 并保存。

`phase_Skeleton` 已确认包含 `UpperBody` 和 `FullBody` 名称。本轮只在 AnimBP 中引用，不打开 Skeleton Slot Manager 修改第三方 Skeleton。

当前没有技能 Montage 时，两个 Slot 都会原样传递 Locomotion 姿势，不应改变基础移动表现。

## 十二、绑定 BP_Hero_Phase 的 Anim Class

1. 打开 `/Game/Blueprints/Characters/BP_Hero_Phase`。
2. 选中 `Mesh`。
3. `Animation Mode` 设置为 `Use Animation Blueprint`。
4. `Anim Class` 设置为 `ABP_Apex_Phase`。
5. Compile 并保存。

不要改变已经验证通过的 Mesh 相对变换、Capsule、CameraSpringArm 或 FollowCamera 参数。

## 十三、编译检查

依次打开并 Compile/Save：

1. `BS_Apex_Phase_Locomotion` 保存。
2. `ABP_Apex_Phase` Compile/Save。
3. `IMC_Apex_BaseMove` 保存。
4. `BP_Hero_Phase` Compile/Save。
5. `BP_ThirdPersonPlayerController` Compile/Save。

AnimBP 必须为绿色编译成功状态。若出现错误，不要带着错误进入 PIE。

## 十四、PIE 验证

使用已经验证过的 `/Game/Blueprints/Maps/Lvl_ThirdPerson` 和现有 GameMode 进入 PIE。

### 1. 输入回归

- W/S 沿相机朝向前后移动。
- A/D 沿相机朝向左右移动。
- 鼠标控制视角。
- Space 正常起跳。
- 不应出现按一次按键触发两次的情况。

### 2. 地面动画

- 静止时播放 Idle。
- 开始移动后平滑过渡到 Jog_Fwd。
- 停止移动后平滑返回 Idle。
- 角色真实最大速度仍为 500 左右；BlendSpace 不应改变移动速度。

### 3. 主动跳跃

- Space 后进入 `JumpStart`。
- 起跳动画结束或开始下降后进入 `Falling`。
- 接触地面后进入 `Land`。
- Land 结束后回到 `Grounded`。

### 4. 走下平台

- 不按 Space，直接从平台边缘走下。
- 应从 `Grounded` 直接进入 `Falling`。
- 不应误播 `JumpStart`。
- 落地后正常播放 `Land`。

### 5. AnimBP 调试

PIE 运行时打开 `ABP_Apex_Phase`：

1. 在顶部 `Debug Filter` 选择当前运行中的 `BP_Hero_Phase` 实例。
2. 打开 `Locomotion` 状态机，观察当前状态的高亮流转。
3. 检查 `GroundSpeed`：静止接近 0，跑动接近 500。
4. 检查 `VerticalSpeed`：起跳为正，下降为负。
5. 检查 `bIsFalling`：离地时为 true，落地后为 false。

### 6. 稳定性

- PIE 不崩溃。
- Output Log 中没有 AnimInstance Owner、MovementComponent 或 Enhanced Input 空引用错误。
- 相机距离、偏移和鼠标视角保持上一轮已验证结果。

## 十五、失败时反馈格式

一次只定位一个问题，不要同时修改多个转换或输入映射。按下面格式告诉 Codex：

```text
失败步骤：
实际现象：
ABP_Apex_Phase 是否编译成功：
PIE Debug Filter 中的当前状态：
GroundSpeed：
VerticalSpeed：
bIsFalling：
相关 Output Log：
```

## 十六、通过后反馈

全部通过后回复：

```text
Phase 动画基线验证通过：项目自有 IA/IMC、Idle/Jog、主动跳跃、走下平台、Falling、Land 和相机输入均正常。
```

收到结果后，Codex 将关闭当前动画冷启动阶段，并返回 Apex GAS 基础 RFC 与第一批技能框架设计。
