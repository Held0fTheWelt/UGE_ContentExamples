#include "UGEPortBlueprintsCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Engine/Blueprint.h"
#include "Engine/Level.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "UGEContentExampleNativeBlueprints.h"

namespace
{
	struct FPortEntry
	{
		const TCHAR* AssetPath;
		UClass* NativeClass;
		bool bFunctionLibrary;
		bool bLevelScript;
	};

	static bool ClearExecutableGraphs(UBlueprint* Blueprint, bool bFunctionLibrary)
	{
		if (!Blueprint)
		{
			return false;
		}
		Blueprint->Modify();
		if (bFunctionLibrary)
		{
			TArray<UEdGraph*> FunctionGraphs = Blueprint->FunctionGraphs;
			for (UEdGraph* Graph : FunctionGraphs)
			{
				if (Graph)
				{
					FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph, EGraphRemoveFlags::MarkTransient);
				}
			}
			TArray<UEdGraph*> MacroGraphs = Blueprint->MacroGraphs;
			for (UEdGraph* Graph : MacroGraphs)
			{
				if (Graph)
				{
					FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph, EGraphRemoveFlags::MarkTransient);
				}
			}
		}
		else
		{
			// Preserve the level EventGraph shell, but remove every executable node.
			for (UEdGraph* Graph : Blueprint->UbergraphPages)
			{
				if (!Graph)
				{
					continue;
				}
				TArray<UEdGraphNode*> Nodes = Graph->Nodes;
				for (UEdGraphNode* Node : Nodes)
				{
					if (Node)
					{
						FBlueprintEditorUtils::RemoveNode(Blueprint, Node, false);
					}
				}
			}
		}
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		return true;
	}

	static bool SaveBlueprint(UBlueprint* Blueprint, const FString& PackageName, bool bMap)
	{
		if (!Blueprint || !Blueprint->GetOutermost())
		{
			return false;
		}
		FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection);
		if (Blueprint->Status == BS_Error)
		{
			return false;
		}
		Blueprint->MarkPackageDirty();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		const FString Filename = FPackageName::LongPackageNameToFilename(
			PackageName, bMap ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
		return UPackage::SavePackage(Blueprint->GetOutermost(), Blueprint, *Filename, SaveArgs);
	}
}

UGEPortBlueprintsCommandlet::UGEPortBlueprintsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
}

int32 UGEPortBlueprintsCommandlet::Main(const FString& Params)
{
	if (Params.Contains(TEXT("-InspectExperiences"), ESearchCase::IgnoreCase))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		const TCHAR* ExperiencePackages[] =
		{
			TEXT("/Game/Experiences/B_DefaultExperience"),
			TEXT("/ShooterCore/Experiences/B_ShooterGame_Elimination"),
			TEXT("/ShooterCore/Experiences/B_LyraShooterGame_ControlPoints")
		};
		const TCHAR* PawnDataPackages[] =
		{
			TEXT("/ShooterCore/Game/HeroData_ShooterGame"),
			TEXT("/Game/Experiences/PawnData_Helicopter")
		};
		for (const TCHAR* PackageName : ExperiencePackages)
		{
			TArray<FAssetData> Assets;
			AssetRegistry.GetAssetsByPackageName(FName(PackageName), Assets, true);
			for (const FAssetData& Asset : Assets)
			{
				FString PrimaryAssetType;
				FString PrimaryAssetId;
				Asset.GetTagValue(FName(TEXT("PrimaryAssetType")), PrimaryAssetType);
				Asset.GetTagValue(FName(TEXT("PrimaryAssetId")), PrimaryAssetId);
				const FPrimaryAssetId ResolvedId = UAssetManager::Get().GetPrimaryAssetIdForPath(Asset.ToSoftObjectPath());
				const FName AssetName = FName(*Asset.AssetName.ToString());
				const FPrimaryAssetId CoreExperienceId(FPrimaryAssetType(TEXT("ExperienceDefinition")), AssetName);
				const FPrimaryAssetId LyraExperienceId(FPrimaryAssetType(TEXT("LyraExperienceDefinition")), AssetName);
				UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: experience package=%s assetClass=%s primaryAssetTypeTag=%s primaryAssetIdTag=%s resolvedPrimaryAssetId=%s"),
					PackageName, *Asset.AssetClassPath.ToString(), *PrimaryAssetType, *PrimaryAssetId, *ResolvedId.ToString());
				UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: experience candidates core=%s corePath=%s lyra=%s lyraPath=%s"),
					*CoreExperienceId.ToString(), *UAssetManager::Get().GetPrimaryAssetPath(CoreExperienceId).ToString(),
					*LyraExperienceId.ToString(), *UAssetManager::Get().GetPrimaryAssetPath(LyraExperienceId).ToString());
				const FSoftObjectPath ExperienceClassPath = UAssetManager::Get().GetPrimaryAssetPath(LyraExperienceId).IsValid()
					? UAssetManager::Get().GetPrimaryAssetPath(LyraExperienceId)
					: UAssetManager::Get().GetPrimaryAssetPath(CoreExperienceId);
				if (UClass* ExperienceClass = Cast<UClass>(ExperienceClassPath.TryLoad()))
				{
					UObject* ExperienceDefault = ExperienceClass->GetDefaultObject();
					for (const TCHAR* PropertyName : { TEXT("DefaultPawnData"), TEXT("PawnData") })
					{
						if (FObjectPropertyBase* ObjectProperty = FindFProperty<FObjectPropertyBase>(ExperienceClass, PropertyName))
						{
							UObject* Value = ObjectProperty->GetObjectPropertyValue_InContainer(ExperienceDefault);
							UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: experience property package=%s property=%s value=%s"),
								PackageName, PropertyName, *GetPathNameSafe(Value));
						}
					}
					if (FArrayProperty* FeaturesProperty = FindFProperty<FArrayProperty>(ExperienceClass, TEXT("GameFeaturesToEnable")))
					{
						FScriptArrayHelper FeaturesHelper(FeaturesProperty, FeaturesProperty->ContainerPtrToValuePtr<void>(ExperienceClass->GetDefaultObject()));
						for (int32 Index = 0; Index < FeaturesHelper.Num(); ++Index)
						{
							if (FStrProperty* FeatureString = CastField<FStrProperty>(FeaturesProperty->Inner))
							{
								UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: experience feature package=%s feature=%s"), PackageName,
									*FeatureString->GetPropertyValue(FeaturesHelper.GetRawPtr(Index)));
							}
						}
					}
				}
			}
		}
		for (const TCHAR* PackageName : PawnDataPackages)
		{
			TArray<FAssetData> Assets;
			AssetRegistry.GetAssetsByPackageName(FName(PackageName), Assets, true);
			for (const FAssetData& Asset : Assets)
			{
				UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: pawnData asset objectPath=%s class=%s"),
					*Asset.ToSoftObjectPath().ToString(), *Asset.AssetClassPath.ToString());
				if (UBlueprint* PawnDataBlueprint = Cast<UBlueprint>(Asset.GetAsset()))
				{
					UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: pawnData package=%s generatedClass=%s parent=%s"), PackageName,
						*GetPathNameSafe(PawnDataBlueprint->GeneratedClass), *GetPathNameSafe(PawnDataBlueprint->ParentClass));
					if (UClass* PawnDataClass = PawnDataBlueprint->GeneratedClass)
					{
						if (FObjectPropertyBase* PawnClassProperty = FindFProperty<FObjectPropertyBase>(PawnDataClass, TEXT("PawnClass")))
						{
							UObject* PawnClass = PawnClassProperty->GetObjectPropertyValue_InContainer(PawnDataClass->GetDefaultObject());
							UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: pawnData package=%s pawnClass=%s"), PackageName, *GetPathNameSafe(PawnClass));
						}
					}
				}
			}
		}
		return 0;
	}

	if (Params.Contains(TEXT("-InspectMaps"), ESearchCase::IgnoreCase))
	{
		const TCHAR* Maps[] =
		{
			TEXT("/ShooterMaps/Maps/L_Convolution_Blockout"),
			TEXT("/ShooterMaps/Maps/L_Expanse"),
			TEXT("/ShooterMaps/Maps/L_Expanse_Blockout"),
			TEXT("/ShooterMaps/Maps/L_FiringRange_WP")
		};
		int32 Failures = 0;
		for (const TCHAR* MapPath : Maps)
		{
			const FString AssetName = FPackageName::GetShortName(MapPath);
			UWorld* World = LoadObject<UWorld>(nullptr, *FString::Printf(TEXT("%s.%s"), MapPath, *AssetName));
			if (!World || !World->PersistentLevel || !World->GetWorldSettings())
			{
				UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: map inspection failed for %s"), MapPath);
				++Failures;
				continue;
			}
			UWorld* InspectionWorld = World;
			AWorldSettings* Settings = World->GetWorldSettings();
			FString DefaultExperience = TEXT("<property unavailable>");
			if (FProperty* ExperienceProperty = Settings->GetClass()->FindPropertyByName(TEXT("DefaultGameplayExperience")))
			{
				if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(ExperienceProperty))
				{
					const FSoftObjectPtr* Value = SoftObjectProperty->ContainerPtrToValuePtr<FSoftObjectPtr>(Settings);
					DefaultExperience = Value ? Value->ToSoftObjectPath().ToString() : TEXT("<null>");
				}
			}
			UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: map=%s worldSettings=%s gameMode=%s defaultGameMode=%s"),
				MapPath,
				*GetNameSafe(Settings->GetClass()),
				*GetPathNameSafe(Settings->DefaultGameMode),
				*GetPathNameSafe(World->GetWorldSettings()->DefaultGameMode));
			UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: map=%s defaultGameplayExperience=%s"), MapPath, *DefaultExperience);
			int32 ActorCount = 0;
			int32 PlayerStarts = 0;
			for (TActorIterator<AActor> It(InspectionWorld); It; ++It)
			{
				++ActorCount;
				if (It->IsA<APlayerStart>())
				{
					++PlayerStarts;
				}
			}
			UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: map=%s actors=%d playerStarts=%d"), MapPath, ActorCount, PlayerStarts);
		}
		return Failures == 0 ? 0 : 1;
	}

	const FPortEntry Entries[] =
	{
		{ TEXT("/ShooterCore/System/Audio/WeaponAudioFunctions"), UWeaponAudioFunctionsNative::StaticClass(), true, false },
		{ TEXT("/Game/Characters/Heroes/B_Hero_Default"), nullptr, false, false },
		{ TEXT("/ShooterCore/Game/HeroData_ShooterGame"), nullptr, false, false },
		{ TEXT("/ShooterMaps/Maps/L_Convolution_Blockout"), AUGELConvolutionBlockoutLevelScript::StaticClass(), false, true },
		{ TEXT("/ShooterMaps/Maps/L_Expanse"), AUGELExpanseLevelScript::StaticClass(), false, true },
		{ TEXT("/ShooterMaps/Maps/L_Expanse_Blockout"), AUGELExpanseBlockoutLevelScript::StaticClass(), false, true },
		{ TEXT("/ShooterMaps/Maps/L_FiringRange_WP"), AUGELFiringRangeWPLevelScript::StaticClass(), false, true }
	};

	int32 Failures = 0;
	for (const FPortEntry& Entry : Entries)
	{
		if (FCString::Stristr(Entry.AssetPath, TEXT("HeroData")))
		{
			UPackage* Package = LoadPackage(nullptr, Entry.AssetPath, LOAD_None);
			UObject* HeroData = Package ? FindObject<UObject>(Package, TEXT("HeroData_ShooterGame")) : nullptr;
			UClass* HeroPawnClass = LoadClass<APawn>(nullptr, TEXT("/Game/Characters/Heroes/B_Hero_Default.B_Hero_Default_C"));
			if (!HeroData || !HeroPawnClass)
			{
				UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: HeroData native asset or hero pawn class unavailable"));
				++Failures;
				continue;
			}
			if (FObjectPropertyBase* PawnClassProperty = FindFProperty<FObjectPropertyBase>(HeroData->GetClass(), TEXT("PawnClass")))
			{
				PawnClassProperty->SetObjectPropertyValue_InContainer(HeroData, HeroPawnClass);
				HeroData->MarkPackageDirty();
				FSavePackageArgs SaveArgs;
				SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
				const FString Filename = FPackageName::LongPackageNameToFilename(Entry.AssetPath, FPackageName::GetAssetPackageExtension());
				const bool bSaved = UPackage::SavePackage(HeroData->GetOutermost(), HeroData, *Filename, SaveArgs);
				if (bSaved)
				{
					UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: HeroData pawn class set to %s (saved=true)"), *HeroPawnClass->GetPathName());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: HeroData pawn class set to %s (saved=false)"), *HeroPawnClass->GetPathName());
				}
				if (!bSaved)
				{
					++Failures;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: HeroData has no native PawnClass property"));
				++Failures;
			}
			continue;
		}
		UClass* NativeClass = Entry.NativeClass;
		if (NativeClass == nullptr)
		{
			NativeClass = FCString::Stristr(Entry.AssetPath, TEXT("HeroData"))
				? LoadClass<UObject>(nullptr, TEXT("/Script/Core_System.PawnData_Core"))
				: LoadClass<AActor>(nullptr, TEXT("/Script/Core_Classes.CoreCharacter"));
		}
		if (NativeClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: native port class unavailable for %s"), Entry.AssetPath);
			++Failures;
			continue;
		}
		UBlueprint* Blueprint = nullptr;
		UWorld* LoadedWorld = nullptr;
		FString SavePackageName = Entry.AssetPath;
		if (Entry.bLevelScript)
		{
			const FString AssetName = FPackageName::GetShortName(Entry.AssetPath);
			LoadedWorld = LoadObject<UWorld>(nullptr, *FString::Printf(TEXT("%s.%s"), Entry.AssetPath, *AssetName));
			if (LoadedWorld && LoadedWorld->PersistentLevel)
			{
				Blueprint = LoadedWorld->PersistentLevel->GetLevelScriptBlueprint(true);
				if (AWorldSettings* WorldSettings = LoadedWorld->GetWorldSettings())
				{
					if (UClass* CoreGameMode = LoadClass<AGameModeBase>(nullptr, TEXT("/Script/Core_Classes.GameMode_Core")))
					{
						WorldSettings->Modify();
						WorldSettings->DefaultGameMode = CoreGameMode;
						WorldSettings->MarkPackageDirty();
						UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: map=%s default game mode set to %s"), Entry.AssetPath, *CoreGameMode->GetPathName());
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: Core GameMode_Core class is unavailable for %s"), Entry.AssetPath);
					}
				}
			}
		}
		else
		{
			const FString AssetName = FPackageName::GetShortName(Entry.AssetPath);
			Blueprint = Cast<UBlueprint>(LoadObject<UObject>(nullptr, *FString::Printf(TEXT("%s.%s"), Entry.AssetPath, *AssetName)));
			if (!Blueprint && FCString::Stristr(Entry.AssetPath, TEXT("HeroData")))
			{
				if (UPackage* Package = LoadPackage(nullptr, Entry.AssetPath, LOAD_None))
				{
					ForEachObjectWithOuter(Package, [&Blueprint](UObject* Object)
					{
						UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: HeroData package object=%s class=%s"),
							*Object->GetPathName(), *Object->GetClass()->GetPathName());
						if (!Blueprint)
						{
							Blueprint = Cast<UBlueprint>(Object);
						}
					});
				}
			}
		}
		if (!Blueprint)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: asset not found %s"), Entry.AssetPath);
			++Failures;
			continue;
		}

		Blueprint->Modify();
		Blueprint->ParentClass = NativeClass;
		if (!ClearExecutableGraphs(Blueprint, Entry.bFunctionLibrary) || !SaveBlueprint(Blueprint, SavePackageName, Entry.bLevelScript))
		{
			UE_LOG(LogTemp, Error, TEXT("UGEPortBlueprints: failed to port %s"), Entry.AssetPath);
			++Failures;
			continue;
		}
		if (FCString::Stristr(Entry.AssetPath, TEXT("HeroData")))
		{
			if (UClass* HeroPawnClass = LoadClass<APawn>(nullptr, TEXT("/Game/Characters/Heroes/B_Hero_Default.B_Hero_Default_C")))
			{
				if (FObjectPropertyBase* PawnClassProperty = FindFProperty<FObjectPropertyBase>(Blueprint->GeneratedClass, TEXT("PawnClass")))
				{
					PawnClassProperty->SetObjectPropertyValue_InContainer(Blueprint->GeneratedClass->GetDefaultObject(), HeroPawnClass);
					Blueprint->MarkPackageDirty();
					SaveBlueprint(Blueprint, SavePackageName, Entry.bLevelScript);
					UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: HeroData pawn class set to %s"), *HeroPawnClass->GetPathName());
				}
			}
		}
		UE_LOG(LogTemp, Display, TEXT("UGEPortBlueprints: ported %s -> %s"), Entry.AssetPath, *NativeClass->GetPathName());
	}

	return Failures == 0 ? 0 : 1;
}
