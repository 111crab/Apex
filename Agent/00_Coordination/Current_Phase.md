# 当前局部阶段

更新日期：2026-07-23

## 顶层阶段

Phase 2 - Apex GAS / 技能系统基础架构。

## 已完成

- 角色、输入、镜头和动画基线已通过 PIE。
- PlayerState Owner / Character Avatar 的 ASC 所有权链已通过单人和多人验证。
- `UApexAttributeSet` 与 `UApexVitalAttributeSet` 已完成编译、复审和单人/多人验证。
- Lyra AbilitySet、GA 基类、InputConfig 和输入队列已完成定向研究。
- 既有 SkillDefinition + AbilityTemplate + AbilityTask + CombatEntity 分层意图已经重新核对。

## 当前任务

已完成并验证：

```text
最小 SkillDefinition
    + GA 根基类
    + AbilitySet 授予
    + Ability 输入配置
    + ASC Pressed/Held/Released 路由
```

## 当前状态

- 用户已批准架构边界、类名和语义输入槽。
- `Current_Code_Design.md` 已形成完整实施预案。
- ClaudeCode 已完成首次实现和两轮定向修复。
- Codex 最终复审通过；资产过滤、PredictionKey、Tag 根域、Avatar 输入清理和完整 Pressed/Held/Released 流程均正确。
- 最终编译结果为 0 error、0 warning。
- UE 项目自有 InputAction、InputConfig、SkillDefinition、AbilitySet 与测试 Ability 已完成配置。
- 单人和多人 PIE 输入验证全部通过。

## 首批输入槽

```text
InputTag.Ability.BasicAttack
InputTag.Ability.Skill1      -> 默认 Q
InputTag.Ability.Skill2      -> 默认 E
InputTag.Ability.Ultimate    -> 默认 X
```

## 本批不做

- 不创建具体投射物、引导或范围技能模板。
- 不实现正式技能、伤害、治疗、GE、Cue、CombatEntity 或 AbilityTask。
- 不重构现有 Move、Look、Jump 输入。
- 不建立完整 HeroDefinition 或技能编辑器。

## 待确认

无。当前代码命名、职责和范围均已确认。

## 下一步

1. 将 GAS 所有权、属性、AbilitySet 与输入路由作为一个完整功能阶段提交并推送。
2. 以真实技能需求设计第一个 GA 模板。
3. 首个模板优先评估 ProjectileCast，用它打通目标、CombatEntity、GE、GameplayCue、动画事件与网络验证。

## Git 建议

本批已验证通过，与当前 GAS 所有权和 Vital AttributeSet 改动一起形成一个完整的“GAS 基础、属性、授予与输入”提交。
