# 决策记录

生成日期：2026-07-14

这里记录新项目长期有效的决策。不要把普通讨论写进来。

## 2026-07-14 - 新项目从干净 UE C++ 项目开始

**决策：** 新项目不直接迁移 Aura 代码，从干净 C++ 项目开始构建技能系统。

**原因：** Aura 承载了教程代码、旧 GAS 路径、中间试验和资产混杂状态，不适合作为长期技能系统地基。

**影响：**
- Aura 只作为经验库。
- 新项目重新设计模块、命名、Tag、Attribute、AbilitySet、SkillDefinition。
- 旧代码只在必要时作为参考，不复制粘贴。

## 2026-07-14 - 先读 Lyra，再写核心架构

**决策：** 新项目正式写技能系统核心代码前，先结构化阅读 Lyra 或同类规范项目。

**原因：** Lyra 在 GAS、输入、AbilitySet、PawnData、GameFeature、GameplayCue 等方面有较成熟实践，适合作为设计参考。

**影响：**
- 第一阶段工作是研究和架构 RFC，不急着写技能。
- 只吸收适合小项目的部分，不照搬全套复杂体系。

## 2026-07-14 - 技能系统采用配置驱动 + 原生模板

**决策：** 长期方向为 `SkillDefinition + AbilityTemplate + CombatEntity + Effect/Buff/State`，而不是每个技能一个完全独立 GA。

**原因：** 这样更适合技能编辑器、复用常见生命周期，并保留 C++ 对网络、预测和性能的控制。

**影响：**
- 技能配置是组装入口，不是万能字段表。
- 特殊机制通过模板、任务、片段或少量 C++ 扩展实现。
- 第一版避免完整 SkillGraph / VM。

## 2026-07-14 - 文档轻量化

**决策：** 新项目只维护少量高价值文档。

**原因：** Aura 阶段文档过多，降低了沟通和执行效率。

**影响：**
- 普通讨论不新建 md。
- 只有长期决策、当前任务、执行 Prompt、实施报告、审查结论、已完成技能链路文档才落盘。

## 2026-07-14 - 非批量 UE 编辑器操作默认人工

**决策：** 单个蓝图创建、设置 Mesh、设置 AnimClass、改 GameMode、添加单个 Notify 等操作默认人工完成，不主动使用 MCP。

**原因：** Aura 阶段验证表明 MCP 对这类需要人眼确认和编辑器状态验证的操作不可靠。

**影响：**
- MCP 主要用于批量扫描、批量检查、批量报告。
- MCP 操作结果必须通过 UE 编辑器可见、可打开、可编译验证。
- ClaudeCode Prompt 必须要求失败时输出人工清单。


## 2026-07-14 - Paragon 资产作为英雄设计基础

**决策：** Apex 第一阶段全面采用 Paragon 资产包中的角色、地图、动画、Montage 和表现资源作为英雄与技能验证基础。

**原因：** 技能系统需要真实的角色动作、地图空间、镜头距离和表现资源约束。Paragon 资产能提供完整第三人称英雄动作语义，避免 Aura 阶段因为动画和表现不足导致技能设计悬空。

**影响：**
- 英雄设计优先从 Paragon 角色和动画出发。
- 技能设计尊重已有 Montage、AnimSequence、Socket、VFX 和地图空间。
- 不为抽象技能硬造不存在的动画；先从可播放的动作语义反推技能候选。
- 资产迁移、角色替换、动画蓝图绑定等非批量 UE 编辑器操作默认给人工步骤，不主动使用 MCP。

## 2026-07-14 - Apex 采用第三人称鼠标视角

**决策：** Apex 采用第三人称项目形态，鼠标负责摄像机视角转动，后续准星、目标选择、投射物、光束和镜头方向都围绕该视角设计。

**原因：** 这个视角既适合 Paragon 英雄资产，也适合验证法术、远程、近战和范围技能的通用框架。

**影响：**
- 输入层优先建立 Move / Look 分离的 Enhanced Input 基线。
- 移动层需要明确角色朝向策略：探索期可先保留模板式移动朝向，技能瞄准期再引入准星/镜头方向约束。
- 先完成基础移动、镜头和动画可播放，再进入 GAS 技能实现。

## 2026-07-14 - Montage 驱动技能候选设计

**决策：** 第一阶段技能和英雄设计采用“先盘点 Montage / 动作语义，再设计技能原型”的流程。

**原因：** Montage 决定了释放姿态、关键帧、Notify 时机和动作可信度。先看资产再定技能，能减少无效设计和后期返工。

**影响：**
- 每个候选英雄先建立 Montage 目录。
- 技能原型要说明依赖哪个 Montage、在哪个关键帧触发 GameplayEvent、需要哪些 Socket / VFX / CombatEntity。
- 长期技能编辑器可以预留 Timeline，但早期仍以 Montage Notify / GameplayEvent 验证技能触发点。

## 2026-07-14 - Aura Agent 上下文迁移为 Apex 协作基线

**决策：** 将 Aura `Agent/` 中的重要协作规则、技能系统讨论、风险清单、技能编辑器愿景、Prompt/报告组织方式迁移到 Apex。

**原因：** Apex 是新项目，但不应丢失 Aura 阶段已经讨论出的工作流程、命名审阅习惯、学习型注释要求、MCP 使用经验和技能系统架构经验。

**影响：**
- Aura 原始上下文完整归档到 `Agent/Aura_Legacy_Archive/`。
- Apex active 文档继续以 `00_Coordination/Decision_Log.md`、`00_Coordination/Task_Backlog.md`、`00_Coordination/Working_Agreement.md`、`00_Coordination/Tooling_Guide.md` 为准。
- Aura 旧类名、旧路径、旧资产不直接迁移；只迁移经验和设计口径。
- 后续如果要继承 Aura 某个设计，必须先写入 Apex 当前文档或 RFC，再实施。

## 2026-07-14 - 参考项目按需读取，不再作为硬前置

**决策：** Lyra、学习包、外部文章和第三方示例项目不再作为每个阶段的主动前置阅读材料。只有当当前问题确实需要参考成熟实现、用户明确要求，或 Prompt 明确要求时才读取。

**原因：** Apex 当前需要先完成项目冷启动、角色基线和资产验证。过早读取大量参考项目会拖慢执行，并可能把过大框架误带入小项目。

**影响：**
- `Project_Roadmap.md` 不把 Lyra 阅读作为硬性 Phase 前置。
- 参考项目阅读必须说明目的、借鉴点和不借鉴点。
- 已有旧决策中“先读 Lyra”改为“需要时读 Lyra 或其他参考项目”。

## 2026-07-14 - 当前阶段与代码设计拆分维护

**决策：** Apex 不使用单一 `Current_Work.md` 混合记录阶段推进和代码设计，而是拆分为 `Current_Phase.md` 与 `Current_Code_Design.md`，并由 `Reviews/` 承接实施后审查。

**原因：** 阶段推进关注下一步、待确认、验证和 Git；代码设计关注实施前约定的大类、关键变量、函数、Tag、属性和配置结构；实施后出现的偏离或新增点应进入审查记录。混在一个文件里会降低日常沟通效率。

**影响：**
- 日常推进和当前待确认优先看 `Agent/00_Coordination/Current_Phase.md`。
- 实施前 C++ / Tag / 属性 / 配置命名预案看 `Agent/00_Coordination/Current_Code_Design.md`。
- `Agent/00_Coordination/Pending_Questions.md` 已删除；当前待确认事项统一写入 `Agent/00_Coordination/Current_Phase.md`。
- 子代理完成代码任务后，Codex 必须把命名表和人工 UE 操作步骤整理给用户审阅。

## 2026-07-14 - 新长期 C++ 代码采用 Public / Private 模块目录

**决策：** Apex 后续新增的长期 gameplay C++ 代码默认使用 UE 模块目录规范：公共头文件放 `Source/Apex/Public/...`，实现文件放 `Source/Apex/Private/...`。

**原因：** 这能让“其他模块或蓝图可见的接口”和“模块内部实现”边界更清楚，也符合 UE 模块长期维护方式。旧模板文件暂时不批量搬迁，避免无意义的大改动。

**影响：**
- 新 `UCLASS` 公共头文件进入 `Public` 后应带 `APEX_API`。
- `.cpp` 引用本模块公共头时优先使用 `Character/ApexPlayerCharacter.h` 这类从 `Public` 根开始的路径。
- 子代理 Prompt 后续应明确 `Public` / `Private` 目标路径。
- 旧模板类和 Variant 示例代码只有在清理阶段再统一处理。

## 2026-07-14 - Agent 活跃文档集中到 00_Coordination

**决策：** Apex `Agent/` 根目录只保留 `README.md`，活跃协作文件统一放到 `Agent/00_Coordination/`。删除 `Pending_Questions.md` 和 `04_AgentHandoff/`，当前待确认事项统一写入 `Current_Phase.md`，子代理交接统一使用 `Prompts/Reports/Reviews`。

**原因：** 根目录散落文档、`Pending_Questions.md` 与 `Current_Phase.md` 双入口、`04_AgentHandoff/` 与 `Prompts/Reports/Reviews` 双入口，会让用户不知道该看哪里。

**影响：**
- 用户日常优先看 `Agent/00_Coordination/Current_Phase.md`。
- 实施前设计看 `Agent/00_Coordination/Current_Code_Design.md`。
- 实施后审查看 `Agent/Reviews/`。
- 长期决策、任务池、协作规则、工具指南都在 `Agent/00_Coordination/`。
- 清理报告见 `Agent/Reports/2026-07-14_Agent_Structure_Cleanup_Report.md`。

## 2026-07-15 - 第三方 Paragon 原始资产不进入普通 Git

**决策：** `Content/ParagonPhase/` 作为本机安装的第三方内容包，不纳入 Apex 普通 Git 提交；项目自建蓝图、地图、配置资产仍需版本控制。

**原因：** Paragon 原始资产体积大、可由资产来源重新安装；但项目自建资产承载角色配置和运行基线，必须进入仓库才能还原项目工作成果。

**影响：**
- `.gitignore` 明确排除 `/Content/ParagonPhase/`。
- `BP_Hero_Phase` 等项目资产可以硬引用该内容包，但文档必须说明外部依赖。
- 从 GitHub 拉取项目后，需要另行安装相同 Paragon Phase 资产才能完整显示角色。

## 2026-07-15 - GAS RFC 前先做 Lyra 定向调研

**决策：** Character 冷启动通过后，先针对 ASC 所有权、初始化生命周期、AbilitySet、InputTag 路由、Ability 激活关系和属性职责研究 Lyra，再编写 Apex GAS RFC。

**原因：** 这些属于 GAS 基础架构的长期决策，若先写代码再补充成熟项目经验，容易重复 Aura 阶段的重构成本。

**影响：**
- Lyra 阅读是当前 GAS RFC 的明确前置，但不是对整个项目永久生效的重型前置阶段。
- 第一轮只研究与 Apex 当前决策直接相关的源码，不完整复制 Experience 或 GameFeature。
- 研究结论必须说明 Apex 采用完整版、轻量版或拒绝，并由用户确认后才能进入实施。
