# Local TTD editor-plugin dependencies

The UE 5.8 ContentExamples editor uses the following Tiny Tool Development
plugins for project tooling. They are intentionally copied into
`EditorPlugins/` locally and ignored by Git; the authoritative source remains
the TinyToolDevelopment workspace. The enabled dependency declarations live in
`UGE_ContentExamples.uproject` and are editor-only.

| Plugin | Authoritative source |
| --- | --- |
| UnrealCapabilityMesh | `D:\TinyToolDevelopment\Git\EnginePlugins\UnrealCapabilityMesh` |
| UnifiedMcpServer | `D:\TinyToolDevelopment\Git\AIPlugins\UnifiedMcpServer` |
| TinyToolExecution | `D:\TinyToolDevelopment\Git\AIPlugins\TinyToolExecution` |
| TinyToolExecutionIntegrationBridge | `D:\TinyToolDevelopment\Git\BridgePlugins\TinyToolExecutionIntegrationBridge` |
| AssetPluginMaker | `D:\TinyToolDevelopment\Git\AIPlugins\AssetPluginMaker` |
| ProjectIntelligenceOrchestrator | `D:\TinyToolDevelopment\Git\AIPlugins\ProjectIntelligenceOrchestrator` |
| UnrealBlueprintIntegrator | `D:\TinyToolDevelopment\Git\AIPlugins\UnrealBlueprintIntegrator` |
| UnrealIntegrationIntelligence | `D:\TinyToolDevelopment\Git\AIPlugins\UnrealIntegrationIntelligence` |
| UnrealCodeIntegrator | `D:\TinyToolDevelopment\Git\AIPlugins\UnrealCodeIntegrator` |
| InternalIndexService | `D:\TinyToolDevelopment\Git\AIPlugins\InternalIndexService` |
| SmartContentDiet | `D:\TinyToolDevelopment\Git\GovernanceDevelopmentPlugins\SmartContentDiet` |

To refresh a local dependency, recopy it from the listed source while excluding
`Binaries/`, `Intermediate/`, and `Saved/`. Do not commit the copied folder.
