# UE Project Context

*Last updated: 2026-07-14*

## Engine & Project Overview

**Engine version:** UE 5.8 (`EngineAssociation`: `5.8`; launcher/source build not confirmed from project files)
**Project name:** `Apex`
**Displayed project name:** `Third Person Game Template`
**Description:** A new UE C++ project intended to become a configuration-driven GAS skill-system prototype; the current codebase still contains UE template variant code.
**Project type:** Game / skill-system R&D prototype
**Genre / domain:** Third-person action/combat prototype now; long-term generic skill-system architecture.
**Target platforms:**
- Desktop is configured as the hardware target.
- Windows uses DX12 / SM6 in `DefaultEngine.ini`.
- Linux Vulkan SM6 and Mac Metal SM6 entries are present.
- Final shipping platforms are not yet established.

## Module Structure

**Primary game module:** `Apex`

| Module | Type | Notes |
|--------|------|-------|
| `Apex` | Runtime | Single game module. Contains root third-person template classes plus `Variant_Combat`, `Variant_Platforming`, and `Variant_SideScrolling` sample-style code. |

**Key dependencies per module:**
- **`Apex`**
  - PublicDeps: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `UMG`, `Slate`
  - PrivateDeps: none currently listed
  - Public include paths are configured for `Apex` and the three variant folders/subfolders.

**Build targets:**
- `ApexTarget`: Game target, `BuildSettingsVersion.V7`, `EngineIncludeOrderVersion.Unreal5_8`
- `ApexEditorTarget`: Editor target, `BuildSettingsVersion.V7`, `EngineIncludeOrderVersion.Unreal5_8`
- No Server or Client target file is currently present.

## Plugin Dependencies

**Engine / project plugins enabled in `Apex.uproject`:**
- `ModelingToolsEditorMode` — editor-only modeling tools
- `StateTree` — StateTree runtime/editor support
- `GameplayStateTree` — gameplay integration for StateTree
- `GASToolsets` — enabled, origin and intended role not yet confirmed

**Modules used but not listed as explicit project plugins:**
- `EnhancedInput` is used by `Apex.Build.cs` and `DefaultInput.ini`.

**Core skill-system plugins not yet established in code/config:**
- `GameplayAbilities`
- `GameplayTasks`
- `Niagara`

**Custom plugins:**
- No project-local `Plugins/` directory exists.

## Coding Conventions

**Naming prefixes:** Standard UE prefixes are used (`A`, `U`, `F`, `I`).
**Header style:** `#pragma once`
**Header organization:** Feature folders under `Source/Apex`; no `Public` / `Private` module split yet.
**UObject references:** Existing code commonly uses `TObjectPtr` for reflected UObject references.
**Log categories in use:**
- `LogApex` — declared in `Apex.h`, defined in `Apex.cpp`
- `LogTemplateCharacter` — declared in template character header; definition not found during the initial scan
- `LogCombatCharacter` — declared in combat character header; definition not found during the initial scan
**Assertion style:** `check()` appears in template/variant code. No project-wide assertion convention is established.
**Additional rules:**
- Treat current template/variant code as starter material, not as final skill-system architecture.
- Do not copy Aura code into this project; Aura is an experience source only.

## Subsystems in Use

**Gameplay framework:**
- Global default map: `/Game/ThirdPerson/Lvl_ThirdPerson.Lvl_ThirdPerson`
- Editor startup map: `/Game/ThirdPerson/Lvl_ThirdPerson.Lvl_ThirdPerson`
- Global default game mode: `/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode.BP_ThirdPersonGameMode_C`
- Root native GameMode: `AApexGameMode`
- Root native PlayerController: `AApexPlayerController`
- Root native Character: `AApexCharacter`
- No native GameState class was found in the initial source scan.

**Template / variant gameplay classes currently present:**
- Combat: `ACombatGameMode`, `ACombatPlayerController`, `ACombatCharacter`, `ACombatEnemy`, `ACombatAIController`, `ACombatEnemySpawner`
- Combat interfaces: `ICombatAttacker`, `ICombatDamageable`, `ICombatActivatable`
- Combat support: combat anim notifies, EQS contexts, StateTree utility structs, `UCombatLifeBar`
- Platforming: `APlatformingGameMode`, `APlatformingPlayerController`, `APlatformingCharacter`, dash notify
- SideScrolling: `ASideScrollingGameMode`, `ASideScrollingPlayerController`, `ASideScrollingCharacter`, `ASideScrollingAIController`, `ASideScrollingNPC`, camera manager, interactable interface, UI and gameplay actors

**Subsystems:**
- No `UGameInstanceSubsystem`, `UWorldSubsystem`, `ULocalPlayerSubsystem`, or `UEngineSubsystem` classes were found.

**GAS usage:**
- Not yet implemented in project source.
- No project classes matching `AbilitySystemComponent`, `GameplayAbility`, `GameplayEffect`, `AttributeSet`, `GameplayTag`, `GameplayCue`, or `AbilityTask` were found in the initial scan.
- Long-term target from `Agent/Context_Migration_From_Aura.md`: `SkillDefinition + AbilityTemplate + CombatEntity + Effect/Buff/State`.

## Build Configuration

**Build targets:** Game and Editor
**Custom macros / build flags:** None found in `Apex.Build.cs` during the initial scan.
**Third-party libraries:** None found in `Apex.Build.cs`.
**Platform-specific notes:**
- Windows config targets DX12 and SM6.
- Linux config targets Vulkan SM6.
- Mac config targets Metal SM6.
**Engine modifications:** Not confirmed. Project files only show `EngineAssociation` as `5.8`.

## Agent / Collaboration Context

**Current role split:**
- User: final decisions, UE editor manual operations, runtime validation, project goal control
- Codex: high-level planning, architecture review, code review, Prompt writing, experience consolidation
- ClaudeCode: concrete code or batch execution under explicit Prompt, with implementation report

**Current project phase:**
- Phase 0 baseline is still in progress.
- Git initialization is not complete in this workspace; `git status` reports this is not a Git repository.
- Compile and PIE status are not verified in this scan.

**Important working constraints:**
- Do not rush into core skill-system implementation before Lyra / reference-project reading and architecture RFC.
- Do not default to MCP for single Blueprint creation, mesh/AnimClass setup, GameMode changes, transform edits, or small Notify edits.
- Long-lived documents only: decisions, tasks, execution prompts, reports, reviews, completed skill runbooks, and reference-project research notes.
