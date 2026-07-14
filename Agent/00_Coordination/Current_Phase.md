# 当前局部阶段

生成日期：2026-07-14
维护规则：这是日常推进入口。它记录当前正在推进的小阶段、执行状态、下一步、待用户确认、人工操作、验证标准和 Git 建议；跨阶段待办放到 `Task_Backlog.md`；实施前代码设计放到 `Current_Code_Design.md`；实施后审阅放到 `Agent/Reviews/`。

## 当前顶层阶段

Phase 0.5 - 角色、Paragon 资产与第三人称基线。

## 当前局部阶段

Character 冷启动：建立长期角色基类和玩家角色类。

## 本阶段目标

- 新增长期使用的角色基础 C++ 类。
- 玩家角色支持第三人称 Camera / SpringArm。
- 支持 Enhanced Input 的 Move / Look / Jump。
- 不接 GAS，不播放 Montage，不写 AnimInstance。
- 为后续绑定 Paragon Phase 蓝图做准备。

## 当前执行状态

| 项 | 状态 |
| --- | --- |
| Codex 规划 | 已完成初版。 |
| 用户命名审阅 | 已确认 `AApexCharacterBase` / `AApexPlayerCharacter`、4 个 C++ 文件路径，以及 `CameraSpringArm` 命名。 |
| ClaudeCode Prompt | 已写入 `Agent/Prompts/2026-07-14_Character_ColdStart_ClaudeCode_Prompt.md`。 |
| ClaudeCode 实施 | 已完成，并写入 `Agent/Reports/2026-07-14_Character_ColdStart_Report.md`。 |
| Codex 目录整理 | 已将新长期代码整理为 `Public/Character/*.h` 与 `Private/Character/*.cpp`。 |
| Rider 项目文件 | 已通过 `Scripts/RegenerateProjectFiles.ps1` 重新生成 `Apex.sln` / `Apex.slnx`。 |
| UE 人工操作清单 | 已写入 `Agent/00_Coordination/Current_UE_Manual_Steps.md`，后续同类操作每次覆盖。 |
| Codex 审查 | 已完成初审：目录/导出宏/include 修正后编译通过；`CameraSpringArm` 改名后二次编译通过。审查见 `Agent/Reviews/2026-07-14_Character_ColdStart_Codex_Review.md`。 |
| UE 人工配置 | 待用户决定 GameMode 策略后手动配置 `BP_Hero_Phase`。 |
| PIE 验证 | 待人工配置完成后进行。 |

## 下一步任务

1. 用户决定默认 Pawn 是先用现有模板 GameMode 临时指定，还是现在新建 Apex 专用 GameMode。
2. 用户按 `Current_UE_Manual_Steps.md` 在 UE 编辑器中人工创建/配置 `BP_Hero_Phase`。
3. PIE 验证基础移动、跳跃和鼠标视角。
4. 阶段稳定后再考虑 commit。

## 待用户确认

当前阶段只剩以下确认点：

- 是否现在由用户手动创建 `BP_Hero_Phase` 并绑定 Paragon Phase Mesh / AnimBP。
- 默认 Pawn 是先用现有模板 GameMode 临时指定，还是现在新建 Apex 专用 GameMode。
- Paragon Phase 是否仍作为第一名验证英雄。
- Phase 自带 AnimBP 是否先直接复用，还是只绑定 Mesh 做僵硬移动验证。

## 人工 UE 编辑器操作草案

确认 GameMode 策略后，预计需要手动做：

1. 创建 `BP_Hero_Phase`，父类选择 `AApexPlayerCharacter`。
2. 设置 Mesh 为 Paragon Phase 的 SkeletalMesh。
3. 设置 Anim Class：第一轮可以为空或使用 Paragon 自带 AnimBP，目标先允许僵硬移动。
4. 配置 `MoveAction`、`LookAction`、`MouseLookAction`、`JumpAction`。
5. 确认 `AApexPlayerController` 的 MappingContext 仍负责添加输入上下文。
6. 在 GameMode 或关卡 World Settings 中设置默认 Pawn 为 `BP_Hero_Phase`。

实际步骤以 Codex 审查后的人工清单为准。

## 验证标准草案

第一轮成功标准：

- `ApexEditor Win64 Development` 编译通过。
- PIE 后生成玩家角色。
- 鼠标能控制摄像机 yaw/pitch。
- WASD 能按摄像机方向移动角色胶囊。
- Jump 能触发跳跃。
- 没有因 InputAction 未配置导致崩溃。
- 不要求动画自然；Mesh 僵硬可接受。

## Git 建议

当前不要急着提交。等用户命名审阅、UE 手动配置和 PIE 验证完成后再分组：

- `docs:` Agent 组织和 Prompt 文档。
- `character:` C++ 角色冷启动代码。
- `assets:` UE 蓝图 / Paragon 配置资产。

如果 UE 资产改动很多，需要先做工作区分类。

## 风险

- 当前工作区已有 `Source/Apex/Public/Character/`、`Source/Apex/Private/Character/`、`Content/ParagonPhase/` 等未跟踪内容，可能来自用户或 ClaudeCode；Codex 不应覆盖。
- 新 C++ 文件直接添加后，Rider / UE 可能需要刷新项目文件或重新编译后才显示。
- 如果 InputAction 没配置，代码必须安全跳过绑定或输出日志，不能崩溃。
