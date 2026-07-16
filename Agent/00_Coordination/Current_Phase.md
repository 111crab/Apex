# 当前局部阶段

生成日期：2026-07-16
维护规则：这是日常推进入口。它记录当前正在推进的小阶段、执行状态、下一步、待用户确认、人工操作、验证标准和 Git 建议；跨阶段待办放到 `Task_Backlog.md`；实施前代码设计放到 `Current_Code_Design.md`；实施后审阅放到 `Agent/Reviews/`。

## 当前顶层阶段

Phase 2 - Apex GAS / 技能系统 RFC。

说明：角色、项目自有输入、镜头和 Phase 动画冷启动均已通过。当前返回架构层，先完成 Apex 专用 GAS 基础 RFC，再决定首批运行时代码。

## 当前局部阶段

Phase 动画基线已完成。当前进入“Apex GAS 基础 RFC 与 Gameplay Framework 职责边界”讨论。

## 本阶段目标

- 明确 ASC 的 Owner / Avatar 归属与初始化时序。
- 明确 Character、PlayerState、PlayerController 和输入组件各自承担的 GAS 职责。
- 确认 GameplayTag 注册与命名规范、Attribute 最小集合和 Ability 授予方式。
- 定义 `SkillDefinition`、Ability 模板、特殊扩展、CombatEntity、Effect 与 State 的首版边界。
- 把现有基础输入迁移方向纳入 `InputConfig + InputTag` 设计，不继续扩张 Character 的 InputAction 成员。
- 只在 RFC 经用户确认后进入 C++ 实施。

## 当前执行状态

| 项 | 状态 |
| --- | --- |
| Codex 规划 | 已完成初版。 |
| 用户命名审阅 | 已确认 `AApexCharacterBase` / `AApexPlayerCharacter`、4 个 C++ 文件路径，以及 `CameraSpringArm` 命名。 |
| ClaudeCode Prompt | 已写入 `Agent/Prompts/2026-07-14_Character_ColdStart_ClaudeCode_Prompt.md`。 |
| ClaudeCode 实施 | 已完成，并写入 `Agent/Reports/2026-07-14_Character_ColdStart_Report.md`。 |
| Codex 目录整理 | 已将新长期代码整理为 `Public/Character/*.h` 与 `Private/Character/*.cpp`。 |
| Rider 项目文件 | AnimInstance 复审通过后已再次运行 `Scripts/RegenerateProjectFiles.ps1`；`Apex.sln` / `Apex.slnx` 生成成功，临时 `Automation_Apex` 已自动清理。 |
| UE 人工操作清单 | 已覆盖为 Phase 动画基线清单，包含 Rider、项目自有 IA/IMC、BlendSpace、AnimBP、状态机、Slot 与 PIE 验证步骤。 |
| Codex 审查 | 已完成初审：目录/导出宏/include 修正后编译通过；`CameraSpringArm` 改名后二次编译通过。审查见 `Agent/Reviews/2026-07-14_Character_ColdStart_Codex_Review.md`。 |
| UE 人工配置 | 已完成 `BP_Hero_Phase`、InputAction、GameMode、SpringArm 与 Camera 配置。 |
| PIE 验证 | 已通过：移动、跳跃、鼠标视角、SpringArm 距离和偏移均正常。 |
| Lyra GAS 定向调研 | 已完成源码核对，第一轮八项基础取舍已由用户确认。 |
| 外部文章交叉分析 | 已完成 Lyra 与漫威争锋两篇文章分析，并形成配置驱动与特殊扩展综合讨论稿。 |
| AnimInstance C++ 实施 | ClaudeCode 已完成并编译通过；报告见 `Agent/Reports/2026-07-16_Apex_AnimInstance_ColdStart_Report.md`。 |
| AnimInstance Codex 审查 | 已完成复审：MovementComponent 重新缓存条件已修复，增量编译通过，当前无代码阻塞。审查见 `Agent/Reviews/2026-07-16_Apex_AnimInstance_ColdStart_Codex_Review.md`。 |
| Phase 动画 UE 配置与 PIE | 已通过：项目自有 IA/IMC、Idle/Jog、主动跳跃、走下平台、Falling、Land 和相机输入均正常。 |

## 下一步任务

1. Codex 基于既有 Lyra/漫威争锋调研与当前 Apex 代码，整理 GAS 基础 RFC 的讨论目录和第一批待确认事项。
2. 先讨论 Gameplay Framework 职责：ASC Owner/Avatar、PlayerState、PlayerController、Character 和输入边界。
3. 再讨论 GameplayTag、Attribute、Ability 授予与输入激活规范。
4. 用户确认基础边界后，再进入 `SkillDefinition + AbilityTemplate + 特殊扩展` 的详细 RFC。

## 待用户确认

Phase 动画基线已经验收关闭。下一批需要用户确认的是 GAS 基础 RFC 中的 Gameplay Framework 类职责与命名；在形成事前代码设计前不新增 C++ 类。

已确认：

- 架构验证样本采用“普通投射物 + 穿透/分裂变体 + 黑洞”。
- SkillDefinition 采用更接近漫威争锋展示的“根资产 + 一个受约束 `FInstancedStruct` 执行配置”。
- 直接建立 Apex 自有 AnimBP，不把 Paragon Phase 原有 AnimBP 作为长期实现。
- 认可 `UApexAnimInstance`、`ABP_Apex_Phase`、`BS_Apex_Phase_Locomotion`、`Locomotion`、`UpperBody`、`FullBody` 第一版命名和范围。
- 项目自建内容统一放在 `/Game/Blueprints/`；动画目录为 `/Game/Blueprints/Characters/Phase/Animation/`。
- 项目自有基础移动 Mapping Context 命名为 `IMC_Apex_BaseMove`。
- `/Game/Blueprints/Input/Actions/IA_*` 是用户自行创建的项目资产，不删除。

下一次 UE 人工操作清单必须包含：把角色四个 InputAction 引用切换到 `/Game/Blueprints/Input/Actions/IA_*`，并同步创建或调整项目自有 Input Mapping Context 的按键映射。只替换 Character 上的 IA 而不修改 IMC，会因为 IMC 仍触发旧 IA 而失去输入。

## 人工 UE 编辑器操作草案

Phase 动画人工清单已经执行并验证通过。进入下一次 UE 人工配置前再覆盖 `Current_UE_Manual_Steps.md`。

## 验证标准草案

当前 RFC 小阶段成功标准：

- 每个 Gameplay Framework 类只有清晰、可解释的职责。
- ASC Owner/Avatar 与单人、Listen Server、重生/换 Pawn 场景兼容。
- GameplayTag 只表达稳定语义，不为临时流程结果盲目加 Tag。
- Attribute 先采用已确认的最小集合，不提前实现复杂伤害公式。
- 技能输入不继续堆叠 Character 成员变量。
- RFC 明确哪些是长远基础、哪些只是 V1 临时实现。

## Git 建议

下一次提交只纳入项目自建资产和对应文档。`Content/ParagonPhase/` 作为本地第三方依赖，不纳入普通 Git 提交。

## 风险

- `BP_Hero_Phase` 硬引用本地 Paragon Phase 资产；从 GitHub 拉取项目后，需要另行安装相同资产包才能完整显示角色。
- 项目地图启用了外部 Actor/对象文件时，提交地图必须同时提交其必要的 `__ExternalActors__` / `__ExternalObjects__` 文件。
- 后续技能输入不能继续无限增加 Character 的 InputAction 成员，应在 GAS RFC 中设计 `InputConfig + InputTag` 层。
- `AApexPlayerCharacter::SetupPlayerInputComponent()` 的父类调用已经补齐并通过编译；后续 UE 验证需确认现有输入行为未发生回归。
