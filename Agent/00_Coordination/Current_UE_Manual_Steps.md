# 当前 UE 编辑器人工操作清单

生成日期：2026-07-15

维护规则：本文只记录当前这一步需要用户在 UE 编辑器中完成的操作。进入新的人工操作阶段时由 Codex 覆盖。

## 当前目标

完成 Phase 玩家角色的第一轮运行基线验收：

- 统一角色蓝图命名。
- 确认实际生成的是新的玩家角色类。
- 验证移动、跳跃和鼠标视角。
- 验证 `CameraSpringArm` 的距离与偏移确实控制当前相机。
- 本轮允许角色保持参考姿势，不要求动画自然。

## 本轮不做

- 不创建 AnimInstance C++ 类。
- 不制作 Locomotion 状态机。
- 不播放 Montage。
- 不接入 GAS。
- 不使用 MCP 执行这些少量编辑器操作。

## 一、在 UE 内统一蓝图命名

当前磁盘中实际存在的资产名是 `BP_Hera_Phase`，已确认统一改为 `BP_Hero_Phase`。

1. 在 Content Browser 中打开 `/Game/Blueprints/Characters/`。
2. 选中 `BP_Hera_Phase`。
3. 按 `F2`，将它重命名为 `BP_Hero_Phase`。
4. 保存资产。
5. 不要在 Windows 文件资源管理器中直接重命名 `.uasset`，必须由 UE 编辑器处理资产引用。
6. 如果目录中出现 Redirector，右键 `/Game/Blueprints/Characters/`，执行 `Fix Up Redirectors in Folder`。

## 二、检查角色蓝图基础配置

打开 `BP_Hero_Phase`。

### 1. 父类

在 `Class Settings` 中确认父类为：

`AApexPlayerCharacter`

如果不是，先停止后续操作并告诉 Codex 当前父类名称，不要自行随意改父类。

### 2. 组件层级

Components 面板应当能够看到以下关系：

```text
CapsuleComponent
├─ Mesh
└─ CameraSpringArm
   └─ FollowCamera
```

重点确认：

- `FollowCamera` 必须是 `CameraSpringArm` 的子组件。
- 不要额外创建第二个 Camera 或 SpringArm。

### 3. Phase Mesh

1. 选中 `Mesh`。
2. `Skeletal Mesh Asset` 选择 Paragon Phase 的角色 Skeletal Mesh。
3. 调整 Mesh 的相对位置和旋转，使脚底大致位于 Capsule 底部，人物朝向与蓝图箭头方向一致。
4. 本轮 `Anim Class` 可以保持为空，允许角色以参考姿势移动。

如果当前已经绑定了能够正常工作的 Phase AnimBP，可以暂时保留，但本轮不把动画自然度作为验收条件。

## 三、填写四个 InputAction

打开 `BP_Hero_Phase` 的 `Class Defaults`，找到 Input 分类，填写：

| 属性 | 资产 | Value Type |
| --- | --- | --- |
| `JumpAction` | `/Game/Input/Actions/IA_Jump` | Boolean |
| `MoveAction` | `/Game/Input/Actions/IA_Move` | Axis2D |
| `LookAction` | `/Game/Input/Actions/IA_Look` | Axis2D |
| `MouseLookAction` | `/Game/Input/Actions/IA_MouseLook` | Axis2D |

注意：

- 角色蓝图只引用 InputAction，不在这里填写 IMC。
- `AApexPlayerCharacter::SetupPlayerInputComponent` 负责把四个 InputAction 绑定到角色函数。
- `BP_ThirdPersonPlayerController` 负责把 Input Mapping Context 添加到本地玩家。

## 四、检查 PlayerController 的 Mapping Context

打开：

`/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController`

在 Class Defaults 中确认：

| 属性 | 应包含的资产 |
| --- | --- |
| `Default Mapping Contexts` | `/Game/Input/IMC_Default` |
| `Mobile Excluded Mapping Contexts` | `/Game/Input/IMC_MouseLook` |

当前模板代码会在本地 PlayerController 初始化输入时添加这些 Mapping Context。这里不需要在角色蓝图的 Event Graph 中再次调用 `Add Mapping Context`。

## 五、设置当前测试关卡和 GameMode

1. 打开 `/Game/Blueprints/Maps/Lvl_ThirdPerson`。
2. 打开 `World Settings`。
3. 将 `GameMode Override` 设置为 `/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode`。
4. 打开 `BP_ThirdPersonGameMode`，确认：
   - `Default Pawn Class` = `BP_Hero_Phase`
   - `Player Controller Class` = `BP_ThirdPersonPlayerController`
5. 确认关卡中存在一个 `PlayerStart`。
6. 检查关卡里是否手动放置了旧玩家角色；如果存在且设置了 `Auto Possess Player = Player 0`，先把该项改为 `Disabled`，避免它抢占控制权。

本轮继续复用现有 `BP_ThirdPersonGameMode`。等角色基线验收完成后，再决定是否建立长期的 Apex 专用 GameMode 资产。

## 六、设置和验证相机

在 `BP_Hero_Phase` 中选中 `CameraSpringArm`。

### 1. 先做明显变化测试

先临时填写：

| 参数 | 测试值 |
| --- | --- |
| `Target Arm Length` | `120` |
| `Socket Offset` | `X=0, Y=80, Z=40` |

编译、保存蓝图并进入 PIE。画面应明显拉近并产生侧向偏移。

如果完全没有变化，优先检查：

1. PIE 实际生成的是否为 `BP_Hero_Phase`。
2. 当前 View Target 是否使用该角色的 `FollowCamera`。
3. `FollowCamera` 是否确实挂在 `CameraSpringArm` 下。
4. 是否编译并保存了蓝图。

### 2. 测试通过后使用临时基线值

结束 PIE，将参数暂时调整为：

| 参数 | 基线值 |
| --- | --- |
| `Target Arm Length` | `260` |
| `Socket Offset` | `X=0, Y=70, Z=35` |
| `Target Offset` | `X=0, Y=0, Z=40` |
| `FollowCamera.Field Of View` | `85` |

这些只是原型观察值，后续可以根据技能瞄准方式重新调整。

不要用 `CameraSpringArm.Relative Rotation` 判断相机链路是否生效。C++ 已开启 `Use Pawn Control Rotation`，运行时 SpringArm 旋转主要由 PlayerController 的 Control Rotation 驱动。

## 七、PIE 验收

编译并保存所有相关蓝图后进入 PIE，逐项验证：

### 1. 生成与控制

- World Outliner 中实际玩家 Pawn 的类是 `BP_Hero_Phase`。
- PlayerController 已 Possess 该角色。
- 场景中没有第二个 Pawn 抢占 `Player 0`。

### 2. 输入

- `W/S`：按当前摄像机 yaw 的前后方向移动。
- `A/D`：按当前摄像机 yaw 的左右方向移动。
- 鼠标横向：改变视角 yaw。
- 鼠标纵向：改变视角 pitch。
- Space：起跳；松开后正常结束跳跃输入。

### 3. 相机

- `Target Arm Length = 120` 的明显测试能改变距离。
- `Socket Offset` 能产生明显侧向偏移。
- 恢复基线值后视角距离合理。
- 相机靠近墙体时可以正常回缩，没有脱离角色。

### 4. 稳定性

- PIE 不崩溃。
- Output Log 中没有 InputAction 为空、Enhanced Input 绑定失败或空指针错误。
- 本轮不要求待机、跑步、跳跃动画自然。

## 八、失败时如何反馈

如果某项失败，请不要一次改动多个地方。按下面格式告诉 Codex：

```text
失败项：例如鼠标不能转动视角
实际 Pawn 类：
实际 PlayerController 类：
BP_Hero_Phase 四个 InputAction 是否均非空：
CameraSpringArm 测试值是否能改变画面：
Output Log 相关警告或错误：
```

## 九、通过后如何反馈

全部通过后，直接回复：

```text
Phase 角色基线验证通过：移动、跳跃、鼠标视角、SpringArm 距离和偏移均正常。
```

Codex 收到后将：

1. 更新 `Current_Phase.md`，关闭 Character 冷启动阶段。
2. 区分项目自建资产与第三方 Paragon 资产。
3. 建议提交 `BP_Hero_Phase`、项目测试地图及其必要外部 Actor 文件。
4. 保持 `Content/ParagonPhase/` 不提交，并记录其为本地第三方依赖。
5. 开始规划 Phase 的最小动画基线。
