# LyraInputConfig preserve shell (S2a)

**Owner:** CE `Plugins/LyraContentCompatibility` (`LyraGame` module) — directory is
gitignored under `Plugins/`; this note + the live editor tree are the durable record.

**Why not ClassRedirect to `InputSystem.InputConfigDataAssetSet`:** UGE InputConfig is a
different primary-asset model (`InputSettingType`, `InputMappingContexts`,
`InputMappingActions`). Lyra `ULyraInputConfig` carries tag-routed
`NativeInputActions` / `AbilityInputActions` (`FLyraInputAction`). Redirecting the class
would drop source fields needed for evidence.

**Applied in editor tree:** `ULyraInputConfig` + `FLyraInputAction` added to
`Plugins/LyraContentCompatibility/Source/LyraGame/Public/LyraContentCompatibilityTypes.h`
(+ constructor in the matching `.cpp`). Rebuild `LyraGame` / `UGE_ContentExamplesEditor`.

**Tag-relationship:** handled by committed `Config/DefaultEngine.ini` CoreRedirects
(not this shell).
