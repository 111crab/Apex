# 当前局部阶段

生成日期：2026-07-15
维护规则：这是日常推进入口。它记录当前正在推进的小阶段、执行状态、下一步、待用户确认、人工操作、验证标准和 Git 建议；跨阶段待办放到 `Task_Backlog.md`；实施前代码设计放到 `Current_Code_Design.md`；实施后审阅放到 `Agent/Reviews/`。

## 当前顶层阶段

Phase 0.5 - 角色、Paragon 资产与第三人称基线。

## 当前局部阶段

Character 冷启动：已完成。当前进入“基线资产收尾与 Lyra GAS 定向调研准备”。

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
| UE 人工配置 | 已完成 `BP_Hero_Phase`、InputAction、GameMode、SpringArm 与 Camera 配置。 |
| PIE 验证 | 已通过：移动、跳跃、鼠标视角、SpringArm 距离和偏移均正常。 |

## 下一步任务

1. 提交 `BP_Hero_Phase`、测试地图、GameMode 改动及其必要外部 Actor；不提交 `Content/ParagonPhase/`。
2. 用户提供本机 Lyra 源码根目录。
3. Codex 按 `Agent/Research_Notes/Lyra_Reading_Plan.md` 完成第一轮 GAS 定向调研。
4. 用户评审 Lyra 借鉴结论后，再进入 Apex GAS RFC。
5. Phase 最小动画基线作为并行的小阶段处理，不阻塞 GAS 架构调研。

## 待用户确认

当前待确认：

- 用户提供 Lyra 源码路径后，确认实际版本和源码范围。
- `/Game/Blueprints/Input/Actions/IA_*` 是未引用的重复资产，本次不提交；后续由用户在 UE 中确认后删除。
- Phase 最小动画基线采用 Paragon AnimBP 还是 Apex 自有 Locomotion AnimBP，稍后单独讨论。

## 人工 UE 编辑器操作草案

Character 冷启动的人工操作已经完成。下一次进入 UE 人工配置前，由 Codex 覆盖 `Current_UE_Manual_Steps.md`。

## 验证标准草案

第一轮成功标准已全部满足：

- `ApexEditor Win64 Development` 编译通过。
- PIE 后生成玩家角色。
- 鼠标能控制摄像机 yaw/pitch。
- WASD 能按摄像机方向移动角色胶囊。
- Jump 能触发跳跃。
- 没有因 InputAction 未配置导致崩溃。
- 不要求动画自然；Mesh 僵硬可接受。

## Git 建议

下一次提交只纳入项目自建资产和对应文档。`Content/ParagonPhase/` 作为本地第三方依赖，不纳入普通 Git 提交。

## 风险

- `BP_Hero_Phase` 硬引用本地 Paragon Phase 资产；从 GitHub 拉取项目后，需要另行安装相同资产包才能完整显示角色。
- 项目地图启用了外部 Actor/对象文件时，提交地图必须同时提交其必要的 `__ExternalActors__` / `__ExternalObjects__` 文件。
- 后续技能输入不能继续无限增加 Character 的 InputAction 成员，应在 GAS RFC 中设计 `InputConfig + InputTag` 层。
- 当前 `AApexPlayerCharacter::SetupPlayerInputComponent()` 缺少 `Super::SetupPlayerInputComponent()`；并入下一次 C++ 实施批次修复。
