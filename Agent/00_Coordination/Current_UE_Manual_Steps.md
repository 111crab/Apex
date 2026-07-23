# 当前 UE 编辑器人工操作

> 状态：代码复审已通过，可以执行本文操作。

更新日期：2026-07-23

## 适用时机

等待 ClaudeCode 实施、编译和 Codex 审查通过后再执行。

## 目标

用一个最小 Blueprint InputProbe GA 验证：

```text
InputAction -> InputTag -> AbilitySpec -> SkillDefinition -> GA 激活
```

这不是正式技能，只用于验证授予与输入基础。

## 启动前

1. 用户运行 `Scripts/RegenerateProjectFiles.ps1`。
2. Rider 中确认新增 GameplayTags、Ability、Data 和 Input 文件均可见。
3. 编译并启动 Apex 编辑器。

## 创建 InputAction

目录：

```text
/Game/Blueprints/Input/Actions/
```

创建四个 Boolean InputAction：

```text
IA_Apex_BasicAttack
IA_Apex_Skill1
IA_Apex_Skill2
IA_Apex_Ultimate
```

## 修改 IMC

打开：

```text
/Game/Blueprints/Input/IMC_Apex_BaseMove
```

增加：

| InputAction | 默认键 |
|---|---|
| `IA_Apex_BasicAttack` | Left Mouse Button |
| `IA_Apex_Skill1` | Q |
| `IA_Apex_Skill2` | E |
| `IA_Apex_Ultimate` | X |

保留所有现有 Move、Look、Jump 映射。

## 创建 AbilityInputConfig

目录：

```text
/Game/Blueprints/Input/
```

创建 DataAsset：

```text
DA_Apex_AbilityInput_Default
```

类型选择 `ApexAbilityInputConfig`，填写四项：

| InputAction | InputTag |
|---|---|
| `IA_Apex_BasicAttack` | `InputTag.Ability.BasicAttack` |
| `IA_Apex_Skill1` | `InputTag.Ability.Skill1` |
| `IA_Apex_Skill2` | `InputTag.Ability.Skill2` |
| `IA_Apex_Ultimate` | `InputTag.Ability.Ultimate` |

运行 Data Validation，必须无错误。

## 创建测试 GA

目录：

```text
/Game/Blueprints/AbilitySystem/Test/
```

创建 Blueprint：

```text
GA_Apex_InputProbe
```

父类选择 `ApexGameplayAbility`。

Class Defaults：

```text
Activation Policy = OnInputTriggered
```

在 `ActivateAbility` 事件中：

1. 调用 `Get Skill Definition`。
2. 对返回对象调用 `Get Display Name`。
3. 调用 `Has Authority` 节点，按返回值分别打印 `Server: <DefinitionName>` 与 `Client: <DefinitionName>`。
4. 调用 `End Ability`。

## 创建四个 SkillDefinition

同一 Test 目录创建：

```text
DA_Skill_Test_BasicAttack
DA_Skill_Test_Skill1
DA_Skill_Test_Skill2
DA_Skill_Test_Ultimate
```

类型选择 `ApexSkillDefinition`，四个资产的：

```text
Ability Template Class = GA_Apex_InputProbe
```

分别运行 Data Validation。

## 创建 Phase Core AbilitySet

目录：

```text
/Game/Blueprints/Characters/Phase/Abilities/
```

创建：

```text
DA_AbilitySet_Phase_Core
```

类型选择 `ApexAbilitySet`，配置：

| SkillDefinition | Level | InputTag |
|---|---:|---|
| `DA_Skill_Test_BasicAttack` | 1 | `InputTag.Ability.BasicAttack` |
| `DA_Skill_Test_Skill1` | 1 | `InputTag.Ability.Skill1` |
| `DA_Skill_Test_Skill2` | 1 | `InputTag.Ability.Skill2` |
| `DA_Skill_Test_Ultimate` | 1 | `InputTag.Ability.Ultimate` |

运行 Data Validation，必须无错误。

## 配置 Phase 角色

打开：

```text
/Game/Blueprints/Characters/Phase/BP_Hero_Phase
```

Class Defaults：

```text
Core Ability Set    = DA_AbilitySet_Phase_Core
Ability Input Config = DA_Apex_AbilityInput_Default
```

编译并保存。

## 单人验证

1. PIE 后执行 `showdebug abilitysystem`，确认四个 AbilitySpec 已授予。
2. 依次按鼠标左键、Q、E、X。
3. 每个按键只激活对应输入槽的 Spec。
4. Print String 能显示对应 SkillDefinition 名称。
5. 移动、跳跃、视角和动画无回归。

## Listen Server + Client 验证

1. `Number of Players = 2`。
2. `Play As Listen Server`。
3. 在服务器玩家和客户端玩家分别按鼠标左键、Q、E、X。
4. 每个本地玩家只能驱动自己的 ASC。
5. LocalPredicted 测试 GA 在本地立即响应，并由服务端完成权威激活。
6. 不得出现重复授予、无效 Spec、空 SourceObject 或输入粘滞。

## 当前验证边界

- InputProbe 只验证授予和输入，不代表正式技能模板。
- 验证通过后可删除 Test 目录资产，或保留到首个正式技能输入闭环完成。
- 本批不验证冷却、消耗、伤害、Montage、GameplayCue 或 CombatEntity。
