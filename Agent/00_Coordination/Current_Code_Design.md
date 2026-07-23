# 当前代码设计预案

更新日期：2026-07-23
状态：已实施，并通过编译、单人和多人 PIE 运行验证。

## 任务

一次完成 Apex 最小技能定义、AbilitySet 授予和 Ability 输入路由：

```text
InputAction
    -> InputTag
    -> AbilitySpec
    -> UApexGameplayAbility
    -> SkillDefinition
```

本批建立正确的长期引用方向，但只做最小运行闭环，不实现具体技能模板。

## 架构边界

```text
未来 HeroDefinition
    -> AbilitySet：角色获得哪些技能、等级和输入槽
        -> SkillDefinition：单个技能使用哪个 GA 模板
            -> UApexGameplayAbility：所有 Apex GA 的根基类
                -> 未来 Projectile / Channel / Area 等模板
```

- `UApexGameplayAbility` 不是万能 GA，只提供项目公共能力。
- `UApexSkillDefinition` 是单个技能的根配置资产。
- `UApexAbilitySet` 是面向 ASC 的只读授予清单，不保存运行时状态。
- InputTag 属于 AbilitySet 的装配上下文，不属于 SkillDefinition。
- AbilitySet 第一版只授予 GA；GE、动态 AttributeSet 后续按真实需求追加。
- AbilityTask、CombatEntity、Effect、Cue 和模板专属 `FInstancedStruct` 配置仍按既有 RFC 后续实现。

## 文件

新增：

```text
Source/Apex/Public/GameplayTags/ApexGameplayTags.h
Source/Apex/Private/GameplayTags/ApexGameplayTags.cpp

Source/Apex/Public/AbilitySystem/Abilities/ApexGameplayAbility.h
Source/Apex/Private/AbilitySystem/Abilities/ApexGameplayAbility.cpp

Source/Apex/Public/AbilitySystem/Data/ApexSkillDefinition.h
Source/Apex/Private/AbilitySystem/Data/ApexSkillDefinition.cpp

Source/Apex/Public/AbilitySystem/ApexAbilitySet.h
Source/Apex/Private/AbilitySystem/ApexAbilitySet.cpp

Source/Apex/Public/Input/ApexAbilityInputConfig.h
Source/Apex/Private/Input/ApexAbilityInputConfig.cpp
```

修改：

```text
Source/Apex/Public/AbilitySystem/ApexAbilitySystemComponent.h
Source/Apex/Private/AbilitySystem/ApexAbilitySystemComponent.cpp
Source/Apex/Public/Character/ApexPlayerCharacter.h
Source/Apex/Private/Character/ApexPlayerCharacter.cpp
Source/Apex/Public/Player/ApexPlayerController.h
Source/Apex/Private/Player/ApexPlayerController.cpp
Source/Apex/Apex.Build.cs
```

## 首批 Native GameplayTag

统一放在：

```cpp
namespace ApexGameplayTags
```

| C++ 变量名 | Tag 字符串 | 语义 |
|---|---|---|
| `InputTag_Ability` | `InputTag.Ability` | Ability 输入根标签，仅用于分类。 |
| `InputTag_Ability_BasicAttack` | `InputTag.Ability.BasicAttack` | 普通攻击槽。 |
| `InputTag_Ability_Skill1` | `InputTag.Ability.Skill1` | 第一个小技能，默认 Q。 |
| `InputTag_Ability_Skill2` | `InputTag.Ability.Skill2` | 第二个小技能，默认 E。 |
| `InputTag_Ability_Ultimate` | `InputTag.Ability.Ultimate` | 大招槽，默认 X。 |

Tag 不包含 Q/E/X 等物理键名。IMC 负责物理按键映射。

不创建 GameplayTag 单例，不加入技能身份、伤害、状态或 InputBlocked Tag。

## EApexAbilityActivationPolicy

```cpp
UENUM(BlueprintType)
enum class EApexAbilityActivationPolicy : uint8
{
    OnInputTriggered,
    WhileInputActive
};
```

- `OnInputTriggered`：按下时尝试激活。引导、蓄力能力激活后仍可接收 Released。
- `WhileInputActive`：按住期间，只要能力当前未激活就持续尝试，适合自动攻击或持续开火。
- `OnSpawn`、被动监听和并发组后续按真实能力加入，本批不预造。

## UApexGameplayAbility

父类：

```cpp
UGameplayAbility
```

类声明：

```cpp
UCLASS(Abstract, Blueprintable)
```

构造默认值：

```text
InstancingPolicy   = InstancedPerActor
NetExecutionPolicy = LocalPredicted
ReplicationPolicy = ReplicateNo
ActivationPolicy  = OnInputTriggered
```

首批函数：

```cpp
EApexAbilityActivationPolicy GetActivationPolicy() const;
UApexAbilitySystemComponent* GetApexAbilitySystemComponentFromActorInfo() const;
AApexPlayerCharacter* GetApexPlayerCharacterFromActorInfo() const;
const UApexSkillDefinition* GetSkillDefinition() const;
```

- `ActivationPolicy` 使用 `EditDefaultsOnly, BlueprintReadOnly`。
- `GetSkillDefinition()` 从当前 AbilitySpec 的 SourceObject 取得定义。
- `GetSkillDefinition()` 必须使用 `UFUNCTION(BlueprintPure, Category = "Apex|Ability")` 暴露给蓝图，供技能蓝图读取当前 SkillDefinition；其余辅助 Getter 是否暴露给蓝图，以实际调用需要为准，避免无意义扩大 API。
- 本批不覆写完整激活、Commit、冷却、消耗、目标或 Montage 流程。
- 未来 `UApexProjectileCastAbility` 等模板直接或经极薄中间层继承该类。

## UApexSkillDefinition

父类：

```cpp
UPrimaryDataAsset
```

类声明：

```cpp
UCLASS(BlueprintType, Const)
```

第一版唯一字段：

```cpp
TSubclassOf<UApexGameplayAbility> AbilityTemplateClass;
```

Getter：

```cpp
TSubclassOf<UApexGameplayAbility> GetAbilityTemplateClass() const;
```

设计说明：

- SkillDefinition 资产本身作为技能身份入口。
- 本批只确定“使用哪个 GA 模板”这一稳定关系。
- Identity、Presentation、目标规则、CombatEntity、Effect/Cue 和受约束的 `FInstancedStruct ExecutionConfig` 后续增量加入。
- 编辑器 Data Validation 检查 `AbilityTemplateClass` 非空。

## FApexAbilitySetAbilityGrant

```cpp
USTRUCT(BlueprintType)
struct FApexAbilitySetAbilityGrant
```

字段：

```cpp
TObjectPtr<UApexSkillDefinition> SkillDefinition;
int32 AbilityLevel = 1;
FGameplayTag InputTag;
```

- `SkillDefinition` 必填。
- `AbilityLevel` 必须大于 0。
- `InputTag` 可空：主动输入能力使用有效 Tag；未来事件触发或被动能力可以不占输入槽。
- 有效 InputTag 必须位于 `InputTag.Ability` 下。

## FApexAbilitySetGrantedHandles

运行时结构，只保存本次 AbilitySet 授予产生的：

```cpp
TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
```

函数：

```cpp
void AddAbilitySpecHandle(FGameplayAbilitySpecHandle Handle);
void TakeFromAbilitySystem(UApexAbilitySystemComponent* ASC);
bool IsEmpty() const;
```

- 该结构由角色运行时实例持有，不写回 AbilitySet 资产。
- `TakeFromAbilitySystem()` 只允许权威端执行，并在撤销后清空 Handle。
- 第一版不保存 GE Handle 或动态 AttributeSet。

## UApexAbilitySet

父类：

```cpp
UPrimaryDataAsset
```

类声明：

```cpp
UCLASS(BlueprintType, Const)
```

配置：

```cpp
TArray<FApexAbilitySetAbilityGrant> GrantedAbilities;
```

执行函数：

```cpp
void GiveToAbilitySystem(
    UApexAbilitySystemComponent* ASC,
    FApexAbilitySetGrantedHandles* OutGrantedHandles) const;
```

授予步骤：

1. 验证 ASC 为权威端。
2. 遍历 Grant 条目并校验 SkillDefinition、模板类、等级和 InputTag。
3. 使用 SkillDefinition 的 `AbilityTemplateClass` 创建 `FGameplayAbilitySpec`。
4. 把 SkillDefinition 设置为 Spec SourceObject。
5. 有效 InputTag 写入 Spec Dynamic Source Tags。
6. 调用 `GiveAbility()`。
7. 把返回的 SpecHandle 写入 `OutGrantedHandles`。

Data Validation 额外检查同一 AbilitySet 中不允许重复的有效 InputTag。

## FApexAbilityInputAction

```cpp
USTRUCT(BlueprintType)
struct FApexAbilityInputAction
```

字段：

```cpp
TObjectPtr<const UInputAction> InputAction;
FGameplayTag InputTag;
```

只允许 `InputTag.Ability` 子 Tag。

## UApexAbilityInputConfig

父类：

```cpp
UDataAsset
```

类声明：

```cpp
UCLASS(BlueprintType, Const)
```

配置：

```cpp
TArray<FApexAbilityInputAction> AbilityInputActions;
```

Getter：

```cpp
const TArray<FApexAbilityInputAction>& GetAbilityInputActions() const;
```

Data Validation 检查：

- InputAction 非空。
- InputTag 有效且属于 `InputTag.Ability`。
- InputAction 不重复。
- InputTag 不重复。

该类只处理 Ability 输入。现有 Jump、Move、Look、MouseLook 成员与绑定保持原样。

## UApexAbilitySystemComponent 输入队列

新增私有成员：

```cpp
TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
```

新增公共函数：

```cpp
void AbilityInputTagPressed(const FGameplayTag& InputTag);
void AbilityInputTagReleased(const FGameplayTag& InputTag);
void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
void ClearAbilityInput();
```

重写：

```cpp
virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
```

行为：

- `AbilityInputTagPressed/Released()` 只匹配 Dynamic Source Tags 并维护输入队列，不在遍历 ActivatableAbilities 时直接激活能力。
- Pressed：匹配 Spec Dynamic Source Tags，加入 Pressed 与 Held。
- Released：加入 Released，并从 Held 移除。
- `ProcessAbilityInput()` 先收集 Held 中待激活的 `WhileInputActive`，再处理 Pressed 状态和 `OnInputTriggered`，统一调用 `TryActivateAbility()`，最后处理 Released。
- Pressed 阶段必须设置 `Spec.InputPressed = true`；Released 阶段必须设置为 `false`。
- 已激活能力收到 `AbilitySpecInputPressed/Released`。
- Pressed/Released 通过 `InvokeReplicatedEvent()` 支持 `WaitInputPress/WaitInputRelease` 等 AbilityTask。
- 每帧结束清空 Pressed/Released，Held 保留到松开。
- 输入队列属于当前 Avatar；撤销 CoreAbilitySet、UnPossessed 或更换 Avatar 时必须调用 `ClearAbilityInput()`，防止 Held Handle 跨角色残留。
- 本批不加入 InputBlocked Tag；`ClearAbilityInput()` 同时作为后续状态/UI 切换入口。

## AApexPlayerController

新增：

```cpp
UApexAbilitySystemComponent* GetApexAbilitySystemComponent() const;
virtual void PostProcessInput(float DeltaTime, bool bGamePaused) override;
```

- 从 `AApexPlayerState` 取得 ASC。
- 只有本地 Controller 调用 `ASC->ProcessAbilityInput()`。
- Controller 只推进输入队列，不决定具体技能或保存战斗状态。

## AApexPlayerCharacter

新增资产成员：

```cpp
TObjectPtr<UApexAbilitySet> CoreAbilitySet;
TObjectPtr<UApexAbilityInputConfig> AbilityInputConfig;
```

属性：

```text
EditDefaultsOnly
BlueprintReadOnly
Category = "Ability System"
```

新增运行时成员：

```cpp
FApexAbilitySetGrantedHandles CoreAbilitySetHandles;
```

新增函数：

```cpp
void GrantCoreAbilitySet();
void RemoveCoreAbilitySet();
void InputAbilityTagPressed(FGameplayTag InputTag);
void InputAbilityTagReleased(FGameplayTag InputTag);
virtual void UnPossessed() override;
virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
```

生命周期：

- `PossessedBy()`：服务器完成 ActorInfo 初始化后，授予 CoreAbilitySet。
- `OnRep_PlayerState()`：客户端只初始化 ActorInfo，不执行授予。
- `SetupPlayerInputComponent()`：保留现有移动/视角/跳跃绑定，再遍历 AbilityInputConfig。
- Ability InputAction 的 `Started` 调用 Pressed。
- `Completed` 与 `Canceled` 调用 Released。
- `UnPossessed()` 和 `EndPlay()` 在权威端幂等撤销本角色授予的 Handle，避免换英雄或重生后残留。
- 服务器无 CoreAbilitySet 时只记录一次明确日志，不崩溃。

`BP_Hero_Phase` 第一版直接配置 CoreAbilitySet 与 AbilityInputConfig。未来 HeroDefinition 建立后只迁移这两个引用，不改变 AbilitySet、SkillDefinition 或输入管线。

## 网络边界

```text
服务器：
    GiveAbility / ClearAbility
    保存权威 AbilitySpec

本地客户端：
    Enhanced Input 产生 Pressed / Released
    ProcessAbilityInput 尝试本地预测激活

GAS：
    根据 GA NetExecutionPolicy 完成预测、服务器确认和输入事件同步
```

- 不在客户端调用 `GiveAbility()`。
- 不在服务端为远端 Pawn 处理本地输入队列。
- AbilitySet 和 SkillDefinition 是只读内容资产，不保存玩家运行时状态。
- 第一版使用 Character Blueprint 的硬引用，随英雄一起加载，不引入异步 AssetManager 管线。

## 本批不做

- 不创建 Projectile、Channel、Area 等空模板类。
- 不创建正式技能、伤害、治疗、GE、Cue、CombatEntity 或 AbilityTask。
- 不创建完整 HeroDefinition。
- 不加入 ActivationGroup、OnSpawn、InputBlocked 或 TagRelationshipMapping。
- 不修改现有移动、跳跃、镜头和动画行为。
- 不使用 MCP 创建 UE 资产。

## 编译和验证

- 子代理执行 `ApexEditor Win64 Development` 编译。
- Codex 审查代码和报告后，用户自行刷新 Rider 项目文件。
- 用户按 `Current_UE_Manual_Steps.md` 创建最小 InputProbe 蓝图和配置资产。
- 单人和 Listen Server + Client 验证四个输入槽都能激活对应 Spec。
