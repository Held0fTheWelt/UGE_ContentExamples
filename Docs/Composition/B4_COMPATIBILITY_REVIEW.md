# Phase 6 B4 Compatibility Review

Status: **draft; explicit human disposition required before B5**

## Prepared composition scope

- WarCollection2 inventory: 121 plugins, including 24 code mirrors against UGE ContentExamples.
- WarCollection draft: 6 selected content packs, 115 explicit evidenced skips, no source code adoption.
- Lyra inventory: 46 plugins, including 7 code mirrors.
- Lyra exact-preservation scan: 7,617 assets, 12,426 physical payload files, 10 maps.
- Lyra packaging plan: `8e81a9e4a3205e12a99a9019f80c4e3d74a06b3d`.
- UGE draft: all 35 source plugins retained as dependency-ordered replacements; one construction-ready experience wiring chapter with four reviewed spec references.

## Blocking Lyra compatibility result

The APM compiler reports 207 blockers:

- 130 `UnresolvedSourceClass`
- 66 `UnresolvedSourceClassImport`
- 7 `UnresolvedClassRedirectTarget`
- 4 `UnresolvedSourceScriptModule`

The scoped closure persists 56 distinct `/Script/LyraGame.*` or `/Script/ShooterCoreRuntime.*` types. The exact list is embedded in `lyra.composition.v1.json` under `compatibility_review.persisted_source_types`. The unresolved source modules are `AnimationLocomotionLibraryRuntime`, `CommonGame`, `LyraGame`, and `ShooterCoreRuntime`.

Four configured target type names do not currently resolve in the target editor (seven blocker occurrences):

- `/Script/GameFeature_InventorySystemRuntime.InventoryItemDefinition`
- `/Script/GameFeature_EquipmentSystemRuntime.EquipmentDefinition`
- `/Script/GameFeature_EquipmentSystemRuntime.WeaponPickupDefinition`
- `/Script/Core_Physics.CorePhysicalMaterialWithTags`

Consequently, exact package preservation is byte- and path-safe but is not yet a 1:1 gameplay proof. No package has been copied and no target asset has been mutated.

## Required human decision

Approve one compatibility disposition before changing any manifest to `approved`:

1. Implement UGE-native compatibility types and redirects for the complete persisted-type set, preserving SAD D6 (UGE remains the only code source). This is the recommended path for the requested Lyra map playability.
2. Narrow the Lyra acceptance scope and regenerate the closure. This changes the requested map/experience scope and cannot be assumed.
3. Permit Lyra runtime code as an additional code source. This conflicts with SAD D6 and requires an architecture decision before implementation.

After the selected implementation removes every packaging blocker, the three manifests still require a separate approval commit. Only then may B5 transfer, B6 verification, B7 Lightning comparison, and B8 interactive PIE evidence run.

## Evidence

- `D:/UGE_ContentExamples/Saved/PioComposition/inventories/wc2-vs-ce.json`
- `D:/UGE_ContentExamples/Saved/PioComposition/inventories/lyra-vs-ce.json`
- `D:/UGE_ContentExamples/Saved/PioComposition/inventories/lyra-exact-source-snapshot.json`
- `D:/UGE_ContentExamples/Saved/PioComposition/lyra-exact-packaging.plan.json`
- `D:/UGE_ContentExamples/Docs/Composition/evidence/lyra-source-pin.v1.json`
- `D:/UGE_ContentExamples/Saved/CodexEvidence/ucm_apm_experience_scaffold_preview_response.json`
