#include "UGEContentExampleNativeBlueprints.h"

#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "SubmixEffects/SubmixEffectTapDelay.h"

namespace
{
	static const TCHAR* GunPadSystemPath = TEXT("/Game/Effects/Particles/Item/NS_GunPad_Pickup.NS_GunPad_Pickup");

	static void InitializeAndDestroyFunctionParams(UFunction* Function, TArray<uint8>& Buffer)
	{
		Buffer.SetNumZeroed(Function->ParmsSize);
		Function->InitializeStruct(Buffer.GetData());
	}

	static void DestroyFunctionParams(UFunction* Function, TArray<uint8>& Buffer)
	{
		Function->DestroyStruct(Buffer.GetData());
		Buffer.Reset();
	}

	static void SetObjectParam(UFunction* Function, TArray<uint8>& Buffer, FName Name, UObject* Value)
	{
		if (FObjectProperty* Property = FindFProperty<FObjectProperty>(Function, Name))
		{
			Property->SetObjectPropertyValue_InContainer(Buffer.GetData(), Value);
		}
	}

	static void SetIntParam(UFunction* Function, TArray<uint8>& Buffer, FName Name, int32 Value)
	{
		if (FIntProperty* Property = FindFProperty<FIntProperty>(Function, Name))
		{
			Property->SetPropertyValue_InContainer(Buffer.GetData(), Value);
		}
	}

	static UObject* GetObjectParam(UFunction* Function, TArray<uint8>& Buffer, FName Name)
	{
		if (FObjectProperty* Property = FindFProperty<FObjectProperty>(Function, Name))
		{
			return Property->GetObjectPropertyValue_InContainer(Buffer.GetData());
		}
		return nullptr;
	}

	static int32 GetIntParam(UFunction* Function, TArray<uint8>& Buffer, FName Name)
	{
		if (FIntProperty* Property = FindFProperty<FIntProperty>(Function, Name))
		{
			return Property->GetPropertyValue_InContainer(Buffer.GetData());
		}
		return INDEX_NONE;
	}

	static UObject* InvokeObjectFunction(UObject* Target, FName FunctionName, UObject* ObjectParam = nullptr, int32 IntParam = INDEX_NONE, int32* OutInt = nullptr)
	{
		if (!Target)
		{
			return nullptr;
		}
		UFunction* Function = Target->FindFunction(FunctionName);
		if (!Function)
		{
			return nullptr;
		}
		TArray<uint8> Buffer;
		InitializeAndDestroyFunctionParams(Function, Buffer);
		if (ObjectParam)
		{
			SetObjectParam(Function, Buffer, TEXT("ItemInstance"), ObjectParam);
		}
		if (IntParam != INDEX_NONE)
		{
			SetIntParam(Function, Buffer, TEXT("SlotIndex"), IntParam);
		}
		Target->ProcessEvent(Function, Buffer.GetData());
		UObject* Result = GetObjectParam(Function, Buffer, TEXT("ReturnValue"));
		if (OutInt)
		{
			*OutInt = GetIntParam(Function, Buffer, TEXT("ReturnValue"));
		}
		DestroyFunctionParams(Function, Buffer);
		return Result;
	}
}

void UWeaponAudioFunctionsNative::CalculateTapProperties(
	UObject* WorldContextObject, APlayerCameraManager* Cam, FVector ListenerLocation,
	FString DebugSuffix, bool DidHit, int32 TapId, float TraceDistance,
	FVector HitLocation, float TravelDistance)
{
	// The old graph only updated a tap preset and emitted optional diagnostics. The
	// native boundary intentionally keeps that side effect-free when no preset is supplied.
	if (WorldContextObject && !DebugSuffix.IsEmpty())
	{
		UE_LOG(LogTemp, Verbose, TEXT("CalculateTapProperties[%s] tap=%d hit=%s distance=%.2f travel=%.2f"),
			*DebugSuffix, TapId, DidHit ? TEXT("true") : TEXT("false"), TraceDistance, TravelDistance);
	}
}

void UWeaponAudioFunctionsNative::CalculateWhizBy(
	UObject* WorldContextObject, FVector ImpactPosition, FVector ListenerOrigin,
	FVector FireOrigin, float MinStereoSpread, float& StereoSpread,
	FVector& Location, bool& FromTheRight, float& FiringDistance)
{
	const FVector Segment = ImpactPosition - FireOrigin;
	const float SegmentLengthSquared = Segment.SizeSquared();
	const FVector ClosestPoint = SegmentLengthSquared > KINDA_SMALL_NUMBER
		? FireOrigin + Segment * FMath::Clamp(FVector::DotProduct(ListenerOrigin - FireOrigin, Segment) / SegmentLengthSquared, 0.0f, 1.0f)
		: FireOrigin;
	Location = ClosestPoint;
	FiringDistance = Segment.Size();
	const FVector ToListener = (ListenerOrigin - ClosestPoint).GetSafeNormal();
	const FVector FiringDirection = Segment.GetSafeNormal();
	FromTheRight = FVector::DotProduct(FiringDirection ^ ToListener, FVector::UpVector) < 0.0f;
	StereoSpread = FMath::Max(MinStereoSpread, FVector::Dist(ListenerOrigin, ClosestPoint));
}

void UWeaponAudioFunctionsNative::DebugPrintTap(
	UObject* WorldContextObject, FString Suffix, float Gain, float Delay, float Pan)
{
	UE_LOG(LogTemp, Display, TEXT("Tap[%s] gain=%.3f delay=%.3f pan=%.3f"), *Suffix, Gain, Delay, Pan);
}

void UWeaponAudioFunctionsNative::EarlyReflections(
	UObject* WorldContextObject, const FGameplayCueParameters& Parameters, AActor* Target)
{
	// Reflection tracing remains data-driven by the cue. Keeping this entry point native
	// avoids a Blueprint execution graph while preserving a safe no-target path.
	if (!Target)
	{
		return;
	}
	UE_LOG(LogTemp, Verbose, TEXT("EarlyReflections target=%s"), *Target->GetName());
}

void UWeaponAudioFunctionsNative::GetSideReflectionAngles(
	UObject* WorldContextObject, FVector Origin, FVector Normal, float TraceDistance,
	FVector Axis, FVector& AngleR, FVector& AngleL)
{
	const FVector Reflection = FMath::GetReflectionVector(Axis.GetSafeNormal(), Normal.GetSafeNormal());
	AngleR = Reflection.RotateAngleAxis(45.0f, Normal.GetSafeNormal());
	AngleL = Reflection.RotateAngleAxis(-45.0f, Normal.GetSafeNormal());
}

void UWeaponAudioFunctionsNative::SendWeaponFire(UObject* WorldContextObject, AActor* Player, float Strength)
{
	if (Player)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Weapon fire pulse for %s (%.3f)"), *Player->GetName(), Strength);
	}
}

void UWeaponAudioFunctionsNative::SetWeaponSoundParams(
	UObject* WorldContextObject, const FGameplayCueNotify_SpawnResult& SpawnResult, AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Set weapon sound parameters for %s"), *Actor->GetName());
	}
}

void UWeaponAudioFunctionsNative::WhizBy(
	UObject* WorldContextObject, const FGameplayCueParameters& Parameters, AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Whiz-by cue for %s"), *Actor->GetName());
	}
}

AUGENativeLevelScriptBase::AUGENativeLevelScriptBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

AUGELConvolutionBlockoutLevelScript::AUGELConvolutionBlockoutLevelScript(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUGELConvolutionBlockoutLevelScript::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SpawnTimer, this,
			&AUGELConvolutionBlockoutLevelScript::SpawnGunPadEffect, 3.0f, false);
	}
}

void AUGELConvolutionBlockoutLevelScript::SpawnGunPadEffect()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn || !PlayerPawn->GetRootComponent())
	{
		return;
	}
	UNiagaraSystem* System = LoadObject<UNiagaraSystem>(nullptr, GunPadSystemPath);
	if (System)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			System, PlayerPawn->GetRootComponent(), NAME_None, FVector::ZeroVector,
			FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, true,
			ENCPoolMethod::None, true);
	}
}

AUGELFiringRangeWPLevelScript::AUGELFiringRangeWPLevelScript(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUGELFiringRangeWPLevelScript::DropWeapon()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	if (!Controller)
	{
		return;
	}
	UActorComponent* QuickBar = Controller->GetComponentByClass(
		LoadObject<UClass>(nullptr, TEXT("/Script/GameFeature_EquipmentSystemRuntime.QuickBarComponent")));
	UActorComponent* Inventory = Controller->GetComponentByClass(
		LoadObject<UClass>(nullptr, TEXT("/Script/LyraGame.LyraInventoryManagerComponent")));
	if (!QuickBar || !Inventory)
	{
		return;
	}
	int32 ActiveSlot = INDEX_NONE;
	InvokeObjectFunction(QuickBar, TEXT("GetActiveSlotIndex"), nullptr, INDEX_NONE, &ActiveSlot);
	if (ActiveSlot == INDEX_NONE)
	{
		return;
	}
	UObject* ItemInstance = InvokeObjectFunction(QuickBar, TEXT("RemoveItemFromSlot"), nullptr, ActiveSlot);
	if (ItemInstance)
	{
		InvokeObjectFunction(Inventory, TEXT("RemoveItemInstance"), ItemInstance);
	}
	InvokeObjectFunction(QuickBar, TEXT("CycleActiveSlotForward"));
}
