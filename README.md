# UGE Content Examples

Native C++ composition target for Unreal Engine 5.8. Blueprints in this project
remain composition/data surfaces; the playable hero, game mode, pawn data,
camera, movement and startup wiring are implemented in native classes.

## Firing Range

Open `ShooterMaps/Maps/L_FiringRange_WP` and start PIE. The map resolves
`GameMode_Core`, loads the ShooterCore experience, spawns
`/Game/Characters/Heroes/B_Hero_Default`, and binds the native camera and
humanoid control components through `AUnifiedPlayerController`.

## Build

```powershell
& 'D:\Engines\UE_5.8\Engine\Build\BatchFiles\Build.bat' `
  UGE_ContentExamplesEditor Win64 Development `
  -Project="$PWD\UGE_ContentExamples.uproject" `
  -NoHotReloadFromIDE -WaitMutex -MaxParallelActions=1
```

The startup path is validated by running the Firing Range with `-NullRHI` and
checking that the log contains `Possess complete` plus registrations for
`CameraControlInterface` and `HumanoidControlInterface` and no critical error.
