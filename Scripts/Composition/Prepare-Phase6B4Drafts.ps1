param(
    [string]$ProjectRoot = "D:/UGE_ContentExamples"
)

$ErrorActionPreference = "Stop"
$saved = Join-Path $ProjectRoot "Saved/PioComposition"
$docs = Join-Path $ProjectRoot "Docs/Composition"
New-Item -ItemType Directory -Force -Path $docs | Out-Null

function Read-Json([string]$Path) {
    Get-Content $Path -Raw | ConvertFrom-Json
}

function Write-Json([object]$Value, [string]$Path) {
    $json = $Value | ConvertTo-Json -Depth 100
    [System.IO.File]::WriteAllText($Path, $json + [Environment]::NewLine, [System.Text.UTF8Encoding]::new($false))
}

function New-Transfer(
    [string]$Name,
    [string]$Action,
    [int]$Order,
    [string]$SkipReason,
    [string[]]$DependsOn,
    [string[]]$EvidenceRefs
) {
    [pscustomobject][ordered]@{
        plugin_name = $Name
        action = $Action
        order = $Order
        skip_reason = $SkipReason
        depends_on = @($DependsOn)
        evidence_refs = @($EvidenceRefs)
    }
}

$wcEvidence = "$ProjectRoot/Saved/PioComposition/inventories/wc2-vs-ce.json"
$wc = Read-Json (Join-Path $saved "warcollection.composition.draft.json")
$wcSelected = [ordered]@{
    Warfare_Assets = @()
    Warfare_Demonstration_Land = @("Warfare_Assets")
    Warefare_NonMoving = @("Warfare_Assets")
    VehicleAssets = @()
    Aircraft_Civil = @()
    Vehicles_Test_Map = @("VehicleAssets")
}
$wcTransfers = [System.Collections.Generic.List[object]]::new()
$order = 0
foreach ($name in $wcSelected.Keys) {
    $wcTransfers.Add((New-Transfer $name "replace" $order "" $wcSelected[$name] @("$wcEvidence#$name")))
    $order++
}
foreach ($plugin in @($wc.inventory.source.plugins | Sort-Object dependency_rank,name)) {
    if ($wcSelected.Contains($plugin.name)) { continue }
    $reason = if ($plugin.content_only) {
        "Out of Phase 6 acceptance scope; candidate for a later manifest revision."
    } else {
        "Code source is UGE (SAD D6); WarCollection2 contributes content packs only (SAD D10)."
    }
    $wcTransfers.Add((New-Transfer $plugin.name "skip" $order $reason @() @("$wcEvidence#$($plugin.name)")))
    $order++
}
$wc.transfer_plan = @($wcTransfers)
$wc.wiring_plan = @()
$wc.verification.boot_checks = [pscustomobject][ordered]@{
    transfer_complete = ""
    references_complete = "/Warfare_Assets"
    map_loads = "/Warfare_Demonstration_Land/Demonstration.Demonstration"
}
$wc.review.status = "draft"
$wc.review.approved_by = ""
$wc.review.evidence_refs = @()
Write-Json $wc (Join-Path $docs "warcollection.composition.v1.json")

$lyraEvidence = "$ProjectRoot/Saved/PioComposition/inventories/lyra-vs-ce.json"
$packagingPlan = "$ProjectRoot/Saved/PioComposition/lyra-exact-packaging.plan.json"
$snapshot = "$ProjectRoot/Saved/PioComposition/inventories/lyra-exact-source-snapshot.json"
$lyra = Read-Json (Join-Path $saved "lyra.composition.draft.json")
$lyraTransfers = [System.Collections.Generic.List[object]]::new()
$lyraTransfers.Add((New-Transfer "Game" "adopt" 0 "" @() @($packagingPlan, "$snapshot#/Game")))
$lyraTransfers.Add((New-Transfer "ShooterCore" "adopt" 1 "" @("Game") @($packagingPlan, "$lyraEvidence#ShooterCore")))
$lyraTransfers.Add((New-Transfer "ShooterMaps" "adopt" 2 "" @("ShooterCore") @($packagingPlan, "$lyraEvidence#ShooterMaps")))
$order = 3
foreach ($plugin in @($lyra.inventory.source.plugins | Sort-Object dependency_rank,name)) {
    if ($plugin.name -in @("ShooterCore", "ShooterMaps")) { continue }
    $reason = if ($plugin.content_only) {
        "Out of Phase 6 acceptance scope; candidate for a later manifest revision."
    } else {
        "Code source is UGE (SAD D6); LyraStarterGame contributes content packs only (SAD D10)."
    }
    $lyraTransfers.Add((New-Transfer $plugin.name "skip" $order $reason @() @("$lyraEvidence#$($plugin.name)")))
    $order++
}
$lyra.transfer_plan = @($lyraTransfers)
$lyra.wiring_plan = @()
$lyra.verification.boot_checks = [pscustomobject][ordered]@{
    transfer_complete = ""
    references_complete = "/ShooterMaps"
    map_loads = "/ShooterMaps/Maps/L_Expanse.L_Expanse"
    experience_resolves = "/ShooterCore/Experiences/B_ShooterGame_Elimination.B_ShooterGame_Elimination_C"
}
$lyra.review.status = "draft"
$lyra.review.approved_by = ""
$lyra.review.evidence_refs = @()
$plan = Read-Json $packagingPlan
$source = Read-Json $snapshot
$persistedTypes = @($source.assets.imported_class_paths | Where-Object { $_ -match '^/Script/(LyraGame|ShooterCoreRuntime)\.' } | Sort-Object -Unique)
$compatibility = [pscustomobject][ordered]@{
    status = "blocked_pending_human_disposition"
    packaging_plan_hash = $plan.plan_hash
    asset_count = @($source.assets).Count
    payload_count = @($source.assets.payload_files).Count
    map_count = @($source.assets | Where-Object { $_.package_extension -eq ".umap" }).Count
    blocker_count = @($plan.blockers).Count
    blocker_counts = [pscustomobject][ordered]@{
        unresolved_source_class = @($plan.blockers | Where-Object { $_.code -eq "UnresolvedSourceClass" }).Count
        unresolved_source_class_import = @($plan.blockers | Where-Object { $_.code -eq "UnresolvedSourceClassImport" }).Count
        unresolved_redirect_target = @($plan.blockers | Where-Object { $_.code -eq "UnresolvedClassRedirectTarget" }).Count
        unresolved_source_script_module = @($plan.blockers | Where-Object { $_.code -eq "UnresolvedSourceScriptModule" }).Count
    }
    persisted_source_types = $persistedTypes
    evidence_refs = @(
        $packagingPlan,
        $snapshot,
        $lyraEvidence,
        "$ProjectRoot/Docs/Composition/evidence/lyra-source-pin.v1.json"
    )
}
$lyra | Add-Member -Force NoteProperty compatibility_review $compatibility
Write-Json $lyra (Join-Path $docs "lyra.composition.v1.json")

$uge = Read-Json (Join-Path $saved "uge.composition.phase6-b4.draft.json")
$uge.verification.boot_checks = [pscustomobject][ordered]@{
    transfer_complete = ""
    modules_load = ""
    references_complete = "/Game"
    experience_resolves = "/Game/Experiences/B_DefaultExperience.B_DefaultExperience_C"
    lightning_axis_consistent = ""
}
$uge.wiring_plan = @(
    [pscustomobject][ordered]@{
        chapter_id = "experiences"
        construction_ready = $true
        evidence_refs = @(
            "$ProjectRoot/Saved/PioComposition/inventories/uge-vs-ce.json",
            "D:/UGE/Saved/UnrealIntegrationIntelligence/evidence/runs/2026-07-16_191324_300000/manifest.json"
        )
        assets = @(
            [pscustomobject][ordered]@{ kind="blueprint_scaffold"; name="B_DefaultExperience"; spec_ref="$ProjectRoot/Docs/Composition/specs/apm-plan-experiences.json" },
            [pscustomobject][ordered]@{ kind="data_asset"; name="PawnData_Helicopter"; spec_ref="$ProjectRoot/Docs/Composition/specs/ubi-upsert-pawndata-helicopter.json" },
            [pscustomobject][ordered]@{ kind="blueprint_mutation"; name="B_DefaultExperience_PawnData"; spec_ref="$ProjectRoot/Docs/Composition/specs/ubi-cdo-experience-pawndata.json" },
            [pscustomobject][ordered]@{ kind="blueprint_mutation"; name="B_DefaultExperience_GameFeatures"; spec_ref="$ProjectRoot/Docs/Composition/specs/ubi-cdo-experience-gamefeatures.json" }
        )
    }
)
$uge.review.status = "draft"
$uge.review.approved_by = ""
$uge.review.evidence_refs = @()
Write-Json $uge (Join-Path $docs "uge.composition.v1.json")

Write-Output "Prepared Phase 6 B4 drafts in $docs"
