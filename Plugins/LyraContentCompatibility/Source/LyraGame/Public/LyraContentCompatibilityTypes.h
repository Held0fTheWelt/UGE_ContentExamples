#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/ActorComponent.h"
#include "Components/Widget.h"
#include "Components/GameFrameworkComponent.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Actors/PlayerStart_Base.h"
#include "GameFeatureAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "CharacterPartTypes.h"
#include "InputAction.h"
#include "InputModifiers.h"
#include "InventoryItemDefinition.h"
#include "Engine/DataAsset.h"
#include "InventoryItemInstance.h"
#include "Abilities/GameplayAbility_FromEquipment.h"
#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "GameModes/CoreGameState.h"
#include "Teams/TeamSubsystem.h"
#include "Phases/CoreGamePhaseSubsystem.h"
#include "NumberPops/NumberPopComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/CancellableAsyncAction.h"
#include "UI/CoreActivatableWidget.h"
#include "CommonUserWidget.h"
#include "LyraContentCompatibilityTypes.generated.h"

class APlayerState;
class UCommonButtonBase;
class UCommonUserWidget;
class UWidget;
class UInventoryItemInstance;
class ULyraEquipmentInstance;
class UTeamDisplayAsset;
class UActorComponent;

UENUM(BlueprintType)
enum class ELyraTeamComparison : uint8
{
    OnSameTeam,
    DifferentTeams,
    InvalidArgument
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamColorObservedAsyncDelegate, bool, bTeamSet, int32, TeamId, const UTeamDisplayAsset*, DisplayAsset);

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraVerbMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite) FGameplayTag Verb;
    UPROPERTY(BlueprintReadWrite) TObjectPtr<UObject> Instigator = nullptr;
    UPROPERTY(BlueprintReadWrite) TObjectPtr<UObject> Target = nullptr;
    UPROPERTY(BlueprintReadWrite) FGameplayTagContainer InstigatorTags;
    UPROPERTY(BlueprintReadWrite) FGameplayTagContainer TargetTags;
    UPROPERTY(BlueprintReadWrite) FGameplayTagContainer ContextTags;
    UPROPERTY(BlueprintReadWrite) double Magnitude = 1.0;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraCharacterPart
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> PartClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SocketName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ECharacterCustomizationCollisionMode CollisionMode = ECharacterCustomizationCollisionMode::NoCollision;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraInteractionDurationMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite) TObjectPtr<AActor> Instigator = nullptr;
    UPROPERTY(BlueprintReadWrite) float Duration = 0.0f;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraTabDescriptor
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName TabId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText TabText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSlateBrush IconBrush;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bHidden = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UCommonButtonBase> TabButtonType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UCommonUserWidget> TabContentType;
    UPROPERTY(Transient) TObjectPtr<UWidget> CreatedTabContentWidget = nullptr;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraAbilityMontageFailureMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite) TObjectPtr<APlayerController> PlayerController = nullptr;
    UPROPERTY(BlueprintReadWrite) FGameplayTagContainer FailureTags;
    UPROPERTY(BlueprintReadWrite) TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraNotificationMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category=Notification) FGameplayTag TargetChannel;
    UPROPERTY(BlueprintReadWrite, Category=Notification) TObjectPtr<APlayerState> TargetPlayer = nullptr;
    UPROPERTY(BlueprintReadWrite, Category=Notification) FText PayloadMessage;
    UPROPERTY(BlueprintReadWrite, Category=Notification) FGameplayTag PayloadTag;
    UPROPERTY(BlueprintReadWrite, Category=Notification) TObjectPtr<UObject> PayloadObject = nullptr;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraAbilitySimpleFailureMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite) TObjectPtr<APlayerController> PlayerController = nullptr;
    UPROPERTY(BlueprintReadWrite) FGameplayTagContainer FailureTags;
    UPROPERTY(BlueprintReadWrite) FText UserFacingReason;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraPlayerResetMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> OwnerPlayerState = nullptr;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraQuickBarSlotsChangedMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category=Inventory) TObjectPtr<AActor> Owner = nullptr;
    UPROPERTY(BlueprintReadOnly, Category=Inventory) TArray<TObjectPtr<UInventoryItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraQuickBarActiveIndexChangedMessage
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category=Inventory) TObjectPtr<AActor> Owner = nullptr;
    UPROPERTY(BlueprintReadOnly, Category=Inventory) int32 ActiveIndex = 0;
};

/** UE 5.4 editor-only Niagara hierarchy metadata is retained as loadable compatibility data. */
UCLASS()
class LYRAGAME_API UUGELyraNiagaraHierarchyRoot : public UObject
{
    GENERATED_BODY()
};

/** UE 5.6 replacement target for the retired GameplayCameras Matinee shake class. */
UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API UUGELyraMatineeCameraShake : public UCameraShakeBase
{
    GENERATED_BODY()
};

/** UE 5.6 replacement target for the retired Matinee shake pattern import. */
UCLASS(EditInlineNew, BlueprintType)
class LYRAGAME_API UUGELyraMatineeCameraShakePattern : public UCameraShakePattern
{
    GENERATED_BODY()
};

/** UE 5.6 replacement target for the retired AIModule PawnActions component. */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class LYRAGAME_API UUGELyraPawnActionsComponent : public UActorComponent
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraContextEffectAnimNotifyAudioSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumeMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PitchMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraContextEffectAnimNotifyTraceSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector TraceStartOffset = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector TraceEndOffset = FVector(0.0, 0.0, -50.0);
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraContextEffectAnimNotifyVFXSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Scale = FVector::OneVector;
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API UAnimNotify_LyraContextEffects : public UAnimNotify
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetParameters(
        FGameplayTag EffectIn,
        FVector LocationOffsetIn,
        FRotator RotationOffsetIn,
        FLyraContextEffectAnimNotifyVFXSettings VFXPropertiesIn,
        FLyraContextEffectAnimNotifyAudioSettings AudioPropertiesIn,
        bool bAttachedIn,
        FName SocketNameIn,
        bool bPerformTraceIn,
        FLyraContextEffectAnimNotifyTraceSettings TracePropertiesIn);
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API UApplyFrontendPerfSettingsAction : public UGameFeatureAction
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API UCircumferenceMarkerWidget : public UWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Lyra|UI")
    void SetRadius(float InRadius) { Radius = InRadius; InvalidateLayoutAndVolatility(); }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lyra|UI")
    float Radius = 0.0f;
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API UHitMarkerConfirmationWidget : public UWidget
{
    GENERATED_BODY()
};

UCLASS(DefaultToInstanced, EditInlineNew)
class LYRAGAME_API UInventoryFragment_ReticleConfig : public UInventoryItemFragment
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UUserWidget>> ReticleWidgetClasses;
};

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraCameraComponent : public UCameraComponent
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraCameraMode_ThirdPerson : public UObject
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Health), meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraHealthComponent : public UGameFrameworkComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Lyra|Health")
    static ULyraHealthComponent* FindHealthComponent(const AActor* Actor);

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
    bool IsDeadOrDying() const { return bDeadOrDying; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lyra|Health")
    bool bDeadOrDying = false;
};

UCLASS(BlueprintType, EditInlineNew)
class LYRAGAME_API ULyraSettingBasedScalar : public UInputModifier
{
    GENERATED_BODY()
};

/**
 * Preserve shell for imported Lyra InputConfig packages (`/Script/LyraGame.LyraInputConfig`).
 * Shape matches Lyra source (tag-routed actions). Not redirected to UInputConfigDataAssetSet —
 * UGE InputConfig is a different primary-asset model (contexts/actions by setting type).
 */
USTRUCT(BlueprintType)
struct LYRAGAME_API FLyraInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
    FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class LYRAGAME_API ULyraInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    ULyraInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FLyraInputAction> NativeInputActions;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FLyraInputAction> AbilityInputActions;
};

UCLASS(BlueprintType, EditInlineNew)
class LYRAGAME_API ULyraInputModifierAimInversion : public UInputModifier
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, EditInlineNew)
class LYRAGAME_API ULyraInputModifierDeadZone : public UInputModifier
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, EditInlineNew)
class LYRAGAME_API ULyraInputModifierGamepadSensitivity : public UInputModifier
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraNumberPopComponent_NiagaraText : public UNumberPopComponent
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ALyraPlayerBotController : public AAIController
{
    GENERATED_BODY()
};

/**
 * Binary-compatible name for imported Lyra casts.  UGE's ACoreGameState owns
 * the actual gameplay state; this class keeps serialized Blueprint casts valid
 * without importing the Lyra runtime.
 */
UCLASS(Config = Game)
class LYRAGAME_API ALyraGameState : public ACoreGameState
{
    GENERATED_BODY()

public:
    ALyraGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "Lyra|GameState")
    UAbilitySystemComponent* GetLyraAbilitySystemComponent() const;
};

/** Serialized Shooter maps still reference Lyra's player-start class by name. */
UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ALyraPlayerStart : public APlayerStart_Base
{
    GENERATED_BODY()

public:
    ALyraPlayerStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

/** Compatibility target for existing Lyra Blueprint library nodes. */
UCLASS()
class LYRAGAME_API ULyraSystemStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DefaultToSelf = "TargetActor", ComponentClass = "/Script/Engine.ActorComponent", DeterminesOutputType = "ComponentClass"))
    static TArray<UActorComponent*> FindComponentsByClass(AActor* TargetActor, TSubclassOf<UActorComponent> ComponentClass, bool bIncludeChildActors = true);
};

/**
 * Watches UGE team assignments while retaining Lyra's async-node name and
 * output shape for imported Blueprints.
 */
UCLASS()
class LYRAGAME_API UAsyncAction_ObserveTeamColors : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Keywords = "Watch"))
    static UAsyncAction_ObserveTeamColors* ObserveTeamColors(UObject* TeamAgent);

    virtual void Activate() override;
    virtual void SetReadyToDestroy() override;

    UPROPERTY(BlueprintAssignable)
    FTeamColorObservedAsyncDelegate OnTeamChanged;

private:
    UFUNCTION()
    void OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

    UFUNCTION()
    void OnDisplayAssetChanged(const UTeamDisplayAsset* DisplayAsset);

    void BroadcastChange(int32 NewTeam, const UTeamDisplayAsset* DisplayAsset);

    TWeakObjectPtr<UObject> TeamAgentObject;
    int32 LastBroadcastTeamId = INDEX_NONE;
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraEquipmentInstance : public UEquipmentInstance
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraWeaponInstance : public ULyraEquipmentInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateFiringTime() { TimeLastInteractedWith = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0; }

    UFUNCTION(BlueprintPure)
    float GetTimeSinceLastInteractedWith() const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
    TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const { return nullptr; }

private:
    double TimeLastInteractedWith = 0.0;
};

/** Original Lyra hierarchy: ranged weapons are weapon instances, not raw equipment. */
UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraRangedWeaponInstance : public ULyraWeaponInstance
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraEquipmentManagerComponent : public UPawnComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Equipment)
    TArray<ULyraEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<ULyraEquipmentInstance> InstanceType) const;

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<ULyraEquipmentInstance>> EquipmentInstances;
};

/** Keeps the original reticle property pin types used by ShooterCore widgets. */
UCLASS(Abstract)
class LYRAGAME_API ULyraReticleWidgetBase : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitializeFromWeapon(ULyraWeaponInstance* InWeapon);

    UFUNCTION(BlueprintImplementableEvent)
    void OnWeaponInitialized();

protected:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ULyraWeaponInstance> WeaponInstance;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ULyraInventoryItemInstance> InventoryInstance;
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraInventoryItemDefinition : public UInventoryItemDefinition
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class LYRAGAME_API ULyraInventoryItemInstance : public UInventoryItemInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = Inventory)
    TSubclassOf<ULyraInventoryItemDefinition> LyraItemDef;
};

UCLASS(BlueprintType, Meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraInventoryManagerComponent : public UPawnComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
    ULyraInventoryItemInstance* AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount = 1);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
    void RemoveItemInstance(ULyraInventoryItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
    ULyraInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<ULyraInventoryItemInstance>> Items;
};

/** Restores Lyra's typed equipment-ability return pin for existing weapon Blueprints. */
UCLASS()
class LYRAGAME_API ULyraGameplayAbility_FromEquipment : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
    ULyraInventoryItemInstance* GetAssociatedItem() const;

    UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
    ULyraEquipmentInstance* GetAssociatedEquipment() const { return nullptr; }
};

UCLASS(Config = Game)
class LYRAGAME_API ALyraPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerController")
    class ALyraPlayerState* GetLyraPlayerState() const;
};

UCLASS(Config = Game)
class LYRAGAME_API ALyraPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
    ALyraPlayerController* GetLyraPlayerController() const;
};

UCLASS()
class LYRAGAME_API ULyraTeamSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (Keywords = "Get"))
    void FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (ExpandEnumAsExecs = ReturnValue))
    ELyraTeamComparison CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const;
};

UCLASS()
class LYRAGAME_API ULyraGamePhaseSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
    bool IsPhaseActive(const FGameplayTag& PhaseTag) const;
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraGameplayAbility_RangedWeapon : public ULyraGameplayAbility_FromEquipment
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    ALyraPlayerController* GetLyraPlayerControllerFromActorInfo() const;

    UFUNCTION(BlueprintCallable)
    void StartRangedWeaponTargeting() {}
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ULyraSettingScreen : public UCoreActivatableWidget
{
    GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class LYRAGAME_API ALyraWeaponSpawner : public AActor
{
    GENERATED_BODY()

public:
    ALyraWeaponSpawner();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lyra|WeaponPickup")
    TObjectPtr<class UCapsuleComponent> CollisionVolume;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lyra|WeaponPickup")
    TObjectPtr<class UStaticMeshComponent> PadMesh;

    UPROPERTY(BlueprintReadOnly, Category = "Lyra|WeaponPickup")
    TObjectPtr<class UStaticMeshComponent> WeaponMesh;

    UPROPERTY(BlueprintReadOnly, Category = "Lyra|WeaponPickup")
    float CoolDownPercentage = 0.0f;

    UFUNCTION(BlueprintNativeEvent, Category = "Lyra|WeaponPickup")
    void PlayPickupEffects();

    UFUNCTION(BlueprintNativeEvent, Category = "Lyra|WeaponPickup")
    void PlayRespawnEffects();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|WeaponPickup")
    static int32 GetDefaultStatFromItemDef(TSubclassOf<ULyraInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag) { return 0; }
};
