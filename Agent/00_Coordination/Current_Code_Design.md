# 当前代码设计预案

生成日期：2026-07-16
状态：用户已批准，等待子代理实施。
维护规则：本文只记录当前实施批次的事前设计。子代理实施后的新增、偏离和问题进入 `Agent/Reviews/`，不回填成事前决定。

## 当前设计主题

Apex 自有 AnimInstance 与 Phase 最小 Locomotion 动画基线。

## 1. 本批目标

建立一条项目自有、可以被后续英雄复用的动画状态数据链：

```text
AApexCharacterBase / CharacterMovement
-> UApexAnimInstance（C++ 状态翻译层）
-> ABP_Apex_Phase（可视化动画图）
-> BS_Apex_Phase_Locomotion / Locomotion 状态机
-> Phase SkeletalMesh Pose
```

本批成功后，Phase 应具备：

- Idle 与前向 Jog 的平滑过渡。
- 主动跳跃、走下边缘、下落和落地动画。
- 保留 `UpperBody` / `FullBody` Montage Slot 接入位置。
- 继续支持现有移动、跳跃和鼠标视角。
- Character 与 IMC 均切换到项目自有 InputAction。

## 2. 本批不做

- 不接入 ASC、GameplayTag 或 `FGameplayTagBlueprintPropertyMap`。
- 不播放技能 Montage，不添加 GameplayEvent Notify。
- 不实现 AimOffset、Turn In Place、Foot IK、Motion Matching。
- 不实现锁定目标或 Strafe 移动模式。
- 不创建通用 AnimationProfile DataAsset 或 Linked Anim Layer 选择系统。
- 不使用 MCP 创建这批少量 UE 资产。
- 不移动或修改 `Content/ParagonPhase/` 原始第三方资产。

## 3. C++ 文件与类

### 3.1 新增文件

| 文件 | 作用 | 审阅状态 |
| --- | --- | --- |
| `Source/Apex/Public/Animation/ApexAnimInstance.h` | 对外可见的 AnimInstance 基类声明。 | 已确认 |
| `Source/Apex/Private/Animation/ApexAnimInstance.cpp` | 状态缓存实现。 | 已确认 |

### 3.2 类设计

| 类名 | 父类 | 职责 | 审阅状态 |
| --- | --- | --- | --- |
| `UApexAnimInstance` | `UAnimInstance` | 将 CharacterMovement 的运行状态整理成 AnimBP 可读取的稳定变量；未来再桥接 GAS Tag。 | 已确认 |

建议声明：

```text
UCLASS(Transient, Blueprintable)
class APEX_API UApexAnimInstance : public UAnimInstance
```

`Transient` 表示运行时实例不作为持久资产保存；`Blueprintable` 允许 `ABP_Apex_Phase` 以它为父类。

本批只依赖现有 `Engine` 模块，不修改 `Apex.Build.cs`。

## 4. C++ 成员变量

### 4.1 内部引用

| 变量 | 类型 | 可见性 | 作用 | 审阅状态 |
| --- | --- | --- | --- | --- |
| `OwningCharacter` | `TObjectPtr<ACharacter>` | Private, Transient | 缓存 `TryGetPawnOwner()` 的角色。 | 已确认 |
| `CharacterMovementComponent` | `TObjectPtr<UCharacterMovementComponent>` | Private, Transient | 缓存角色移动组件，避免 AnimBP 每帧重复查询。 | 已确认 |

使用 `ACharacter` 而不是 `AApexPlayerCharacter`，使 AI、敌人和其他 `ACharacter` 也能复用该 AnimInstance。

### 4.2 AnimBP 只读状态

| 变量 | 类型 | 计算方式 | AnimBP 用途 | 审阅状态 |
| --- | --- | --- | --- | --- |
| `GroundSpeed` | `float` | `Velocity.Size2D()` | 驱动 1D Locomotion BlendSpace。 | 已确认 |
| `VerticalSpeed` | `float` | `Velocity.Z` | 区分上升跳跃与直接下落。 | 已确认 |
| `bHasAcceleration` | `bool` | 当前加速度二维长度是否大于容差 | 后续区分移动输入与惯性滑动；本批可用于调试。 | 已确认 |
| `bIsFalling` | `bool` | `CharacterMovementComponent->IsFalling()` | 驱动 Grounded / Jump / Falling / Land 转换。 | 已确认 |

这些变量使用：

```text
UPROPERTY(Transient, BlueprintReadOnly, Category="Animation|Locomotion")
```

本批不添加 `MovementDirection`。当前角色启用 `bOrientRotationToMovement=true`，人物会朝向移动方向，方向变量对 1D 前向 Locomotion 没有实际用途。建立锁定/瞄准 Strafe 模式时再添加。

## 5. C++ 函数

| 函数 | 类型 | 作用 | 审阅状态 |
| --- | --- | --- | --- |
| `NativeInitializeAnimation()` | `UAnimInstance` Override | 调用 `Super`，缓存 Owner 与 MovementComponent。 | 已确认 |
| `NativeUpdateAnimation(float DeltaSeconds)` | `UAnimInstance` Override | 调用 `Super`，必要时重新缓存引用，然后更新四个基础状态变量。 | 已确认 |
| `CacheOwnerReferences()` | Private Helper | 集中执行 Owner / MovementComponent 查找，兼容 PIE、编辑器预览和重新初始化。 | 已确认 |
| `ResetLocomotionState()` | Private Helper | Owner 无效时将动画状态恢复安全默认值，避免预览或切换 Pawn 后残留旧值。 | 已确认 |

### 5.1 更新原则

- `NativeUpdateAnimation` 在安全的游戏线程阶段读取 UObject / CharacterMovement 状态。
- AnimGraph 只读取已缓存的 float / bool，不在蓝图 EventGraph 每帧 Cast Character。
- 所有 Override 必须先调用 `Super`。
- Owner 在 AnimBP 编辑器预览中可能为空，空引用是合法状态，不能 `check()` 崩溃。
- 本批没有委托和外部注册，不需要额外清理函数。

## 6. 同批窄修复

在 `AApexPlayerCharacter::SetupPlayerInputComponent()` 开头补充：

```cpp
Super::SetupPlayerInputComponent(PlayerInputComponent);
```

这是 UE Framework Override 的标准父类调用，已在 Character 冷启动审查中记录。本批不重构其余输入代码。

## 7. UE 项目自有资产

### 7.1 建议目录

```text
/Game/Blueprints/Characters/Phase/Animation/
```

项目自建内容统一放在 `/Game/Blueprints/`；该目录可以引用但不修改 `/Game/ParagonPhase/`。

### 7.2 资产命名

| 资产 | 引擎类型 | 作用 | 审阅状态 |
| --- | --- | --- | --- |
| `ABP_Apex_Phase` | Animation Blueprint，父类 `UApexAnimInstance` | Phase Skeleton 的 Apex 主动画图。 | 已确认 |
| `BS_Apex_Phase_Locomotion` | `UBlendSpace1D` 资产 | 按 `GroundSpeed` 混合 Idle / Jog Forward。 | 已确认 |
| `IMC_Apex_BaseMove` | Input Mapping Context | 将基础移动、视角和跳跃按键映射到项目自有 IA。 | 已确认 |

`BS_Apex_Phase_Locomotion` 和状态机都在 UE 编辑器中手动创建，不编写对应 C++ 子类。

Phase 原包现有 BlendSpace 是 Jog Lean、Slope Lean 和 Turn In Place 等专用资产，没有职责匹配的 Idle / Jog Forward 速度型 Locomotion BlendSpace。本批自行创建，避免复制不匹配的轴、采样点和用途；它只改变表现混合，不改变 CharacterMovement 控制。

## 8. BlendSpace 设计

`BS_Apex_Phase_Locomotion`：

| 设置 | 值 |
| --- | --- |
| Skeleton | `phase_Skeleton` |
| Axis Name | `GroundSpeed` |
| Minimum Axis Value | `0` |
| Maximum Axis Value | `500`，对应当前 `MaxWalkSpeed` |
| Sample at 0 | Phase `Idle` |
| Sample at 500 | Phase `Jog_Fwd` |

它只计算表现姿势，不修改 Character 的真实速度。

## 9. AnimBP 与状态机设计

`ABP_Apex_Phase`：

- Target Skeleton：`phase_Skeleton`。
- Parent Class：`UApexAnimInstance`。
- EventGraph：保持为空，不重复计算 C++ 已提供的状态。
- State Machine：`Locomotion`。

状态：

| 状态 | 动画 |
| --- | --- |
| `Grounded` | `BS_Apex_Phase_Locomotion` |
| `JumpStart` | Phase `Jump_Start`，不循环 |
| `Falling` | Phase `Jump_Apex`；进入 UE 后预览确认循环或保持姿势是否自然 |
| `Land` | Phase `Jump_Land`，不循环 |

转换规则：

| 转换 | 条件 |
| --- | --- |
| `Grounded -> JumpStart` | `bIsFalling && VerticalSpeed > 0` |
| `Grounded -> Falling` | `bIsFalling && VerticalSpeed <= 0`，覆盖走下悬崖 |
| `JumpStart -> Falling` | JumpStart 动画即将结束，或已开始下降 |
| `Falling -> Land` | `!bIsFalling` |
| `Land -> Grounded` | Land 动画即将结束 |

### 9.1 AnimGraph 输出顺序

建议稳定结构：

```text
Locomotion State Machine
-> Save Cached Pose "LocomotionPose"
-> UpperBody Slot + Layered Blend Per Bone（从 spine_01）
-> FullBody Slot
-> Output Pose
```

Phase 原始 Skeleton 已包含 `UpperBody` 和 `FullBody` Slot；第一版只复用，不修改第三方 Skeleton。技能 Montage 尚未接入，因此本批只验证图可编译和 Locomotion 正常。

## 10. 项目自有 IA / IMC

保留并使用：

```text
/Game/Blueprints/Input/Actions/IA_Jump
/Game/Blueprints/Input/Actions/IA_Move
/Game/Blueprints/Input/Actions/IA_Look
/Game/Blueprints/Input/Actions/IA_MouseLook
```

在 `/Game/Blueprints/Input/` 新建 `IMC_Apex_BaseMove`，包含键盘、鼠标和手柄的基础移动映射。稍后的 UE 人工清单会明确每个 Modifier。

人工配置必须同时完成：

1. `BP_Hero_Phase` 的四个 InputAction 成员切换到项目自有 IA。
2. `IMC_Apex_BaseMove` 的 Mapping 指向同一批项目自有 IA。
3. 当前 PlayerController 的 `DefaultMappingContexts` 改用 `IMC_Apex_BaseMove`。

## 11. 实施与审查顺序

1. 用户审阅本文的类名、文件、变量、函数、目录和 UE 资产命名。
2. Codex 根据确认结果编写 ClaudeCode 实施 Prompt。
3. ClaudeCode 只写 `UApexAnimInstance` C++ 和 `Super` 窄修复，并提交实施报告。
4. Codex 审查新增类、函数、成员、注释和编译风险。
5. 用户确认代码审查结果。
6. 运行 `Scripts/RegenerateProjectFiles.ps1`，让 Rider 识别新增文件。
7. 编译 `ApexEditor Win64 Development`。
8. Codex 覆盖 `Current_UE_Manual_Steps.md`。
9. 用户在 UE 中手动创建 BlendSpace、AnimBP、状态机、IMC 并完成 PIE 验证。

## 12. 验证标准

### C++ 阶段

- `ApexEditor Win64 Development` 编译通过。
- Rider 能看到 `Public/Animation` 与 `Private/Animation` 新文件。
- AnimInstance 在无 Owner 的编辑器预览环境不会崩溃。

### UE / PIE 阶段

- `BP_Hero_Phase.Mesh.AnimClass = ABP_Apex_Phase`。
- Idle 与 Jog 根据速度平滑混合。
- 主动跳跃播放 JumpStart / Falling / Land。
- 走下平台能直接进入 Falling，不错误播放主动起跳。
- WASD、鼠标视角和 Space 使用项目自有 IA / IMC 后仍正常。
- 单人 PIE 不崩溃、无空引用错误。

## 13. 用户确认结果

以下事项已于 2026-07-16 批准：

1. 新增 `UApexAnimInstance` 及两个文件路径。
2. 四个状态变量、两个内部引用和四个函数名称。
3. 项目自有动画目录改为 `/Game/Blueprints/Characters/Phase/Animation/`。
4. 使用 `ABP_Apex_Phase`、`BS_Apex_Phase_Locomotion`、`IMC_Apex_BaseMove`。
5. 同批补上 `SetupPlayerInputComponent()` 的 `Super` 调用。
