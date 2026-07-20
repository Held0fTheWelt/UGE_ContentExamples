#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "GameplayCueNotifyTypes.h"
#include "Engine/LevelScriptActor.h"
#include "UGEContentExampleNativeBlueprints.generated.h"

class APlayerCameraManager;
class UAudioComponent;
class USubmixEffectTapDelayPreset;
struct FTapDelayInfo;

/** Native implementation for the former ShooterCore/System/Audio/WeaponAudioFunctions graph library. */
UCLASS()
class UGE_CONTENTEXAMPLES_API UWeaponAudioFunctionsNative final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void CalculateTapProperties(
		UObject* WorldContextObject, APlayerCameraManager* Cam, FVector ListenerLocation,
		FString DebugSuffix, bool DidHit, int32 TapId, float TraceDistance,
		FVector HitLocation, float TravelDistance);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void CalculateWhizBy(
		UObject* WorldContextObject, FVector ImpactPosition, FVector ListenerOrigin,
		FVector FireOrigin, float MinStereoSpread, float& StereoSpread,
		FVector& Location, bool& FromTheRight, float& FiringDistance);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void DebugPrintTap(
		UObject* WorldContextObject, FString Suffix, float Gain, float Delay, float Pan);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void EarlyReflections(
		UObject* WorldContextObject, const FGameplayCueParameters& Parameters, AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void GetSideReflectionAngles(
		UObject* WorldContextObject, FVector Origin, FVector Normal, float TraceDistance,
		FVector Axis, FVector& AngleR, FVector& AngleL);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void SendWeaponFire(UObject* WorldContextObject, AActor* Player, float Strength = 0.01f);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void SetWeaponSoundParams(
		UObject* WorldContextObject, const FGameplayCueNotify_SpawnResult& SpawnResult,
		AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="Weapon Audio", meta=(WorldContext="WorldContextObject"))
	static void WhizBy(
		UObject* WorldContextObject, const FGameplayCueParameters& Parameters, AActor* Actor);
};

/** Shared native level-script behavior. Blueprint level scripts contain no executable graph after porting. */
UCLASS(Abstract)
class UGE_CONTENTEXAMPLES_API AUGENativeLevelScriptBase : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AUGENativeLevelScriptBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UGE_CONTENTEXAMPLES_API AUGELConvolutionBlockoutLevelScript final : public AUGENativeLevelScriptBase
{
	GENERATED_BODY()

public:
	AUGELConvolutionBlockoutLevelScript(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

private:
	void SpawnGunPadEffect();
	FTimerHandle SpawnTimer;
};

UCLASS()
class UGE_CONTENTEXAMPLES_API AUGELExpanseLevelScript final : public AUGENativeLevelScriptBase
{
	GENERATED_BODY()
};

UCLASS()
class UGE_CONTENTEXAMPLES_API AUGELExpanseBlockoutLevelScript final : public AUGENativeLevelScriptBase
{
	GENERATED_BODY()
};

UCLASS()
class UGE_CONTENTEXAMPLES_API AUGELFiringRangeWPLevelScript final : public AUGENativeLevelScriptBase
{
	GENERATED_BODY()

public:
	AUGELFiringRangeWPLevelScript(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Native replacement for the former DropWeapon custom event. */
	UFUNCTION(BlueprintCallable, Category="Firing Range")
	void DropWeapon();
};
