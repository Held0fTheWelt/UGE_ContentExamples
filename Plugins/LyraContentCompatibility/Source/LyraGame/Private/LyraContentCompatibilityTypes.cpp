#include "LyraContentCompatibilityTypes.h"

#include "Components/CoreAbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Interfaces/TeamAgentInterface.h"
#include "Teams/TeamDisplayAsset.h"
#include "Teams/TeamSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

ULyraInputConfig::ULyraInputConfig(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAnimNotify_LyraContextEffects::SetParameters(
    FGameplayTag EffectIn,
    FVector LocationOffsetIn,
    FRotator RotationOffsetIn,
    FLyraContextEffectAnimNotifyVFXSettings VFXPropertiesIn,
    FLyraContextEffectAnimNotifyAudioSettings AudioPropertiesIn,
    bool bAttachedIn,
    FName SocketNameIn,
    bool bPerformTraceIn,
    FLyraContextEffectAnimNotifyTraceSettings TracePropertiesIn)
{
    // Imported modifier assets use this editor-facing setup node.  The current
    // UGE notify has no runtime effect payload, so retaining the signature is
    // sufficient and deliberately side-effect free.
}

ULyraHealthComponent* ULyraHealthComponent::FindHealthComponent(const AActor* Actor)
{
    return Actor ? Actor->FindComponentByClass<ULyraHealthComponent>() : nullptr;
}

float ULyraWeaponInstance::GetTimeSinceLastInteractedWith() const
{
    const UWorld* World = GetWorld();
    return World ? FMath::Max(0.0f, static_cast<float>(World->GetTimeSeconds() - TimeLastInteractedWith)) : 0.0f;
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::AddItemDefinition(
    TSubclassOf<ULyraInventoryItemDefinition> ItemDef,
    int32 StackCount)
{
    if (ItemDef == nullptr || StackCount <= 0)
    {
        return nullptr;
    }

    ULyraInventoryItemInstance* Item = NewObject<ULyraInventoryItemInstance>(this);
    Item->LyraItemDef = ItemDef;
    Items.Add(Item);
    return Item;
}

void ULyraInventoryManagerComponent::RemoveItemInstance(ULyraInventoryItemInstance* ItemInstance)
{
    Items.Remove(ItemInstance);
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::FindFirstItemStackByDefinition(
    TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
    for (ULyraInventoryItemInstance* Item : Items)
    {
        if (Item != nullptr && Item->LyraItemDef == ItemDef)
        {
            return Item;
        }
    }
    return nullptr;
}

ULyraInventoryItemInstance* ULyraGameplayAbility_FromEquipment::GetAssociatedItem() const
{
    return nullptr;
}

TArray<ULyraEquipmentInstance*> ULyraEquipmentManagerComponent::GetEquipmentInstancesOfType(
    TSubclassOf<ULyraEquipmentInstance> InstanceType) const
{
    TArray<ULyraEquipmentInstance*> Result;
    for (ULyraEquipmentInstance* Instance : EquipmentInstances)
    {
        if (Instance != nullptr && (InstanceType == nullptr || Instance->IsA(InstanceType)))
        {
            Result.Add(Instance);
        }
    }
    return Result;
}

void ULyraReticleWidgetBase::InitializeFromWeapon(ULyraWeaponInstance* InWeapon)
{
    WeaponInstance = InWeapon;
    InventoryInstance = InWeapon ? Cast<ULyraInventoryItemInstance>(InWeapon->GetInstigator()) : nullptr;
    OnWeaponInitialized();
}

ALyraPlayerState* ALyraPlayerController::GetLyraPlayerState() const
{
    return Cast<ALyraPlayerState>(PlayerState);
}

ALyraPlayerController* ALyraPlayerState::GetLyraPlayerController() const
{
    return Cast<ALyraPlayerController>(GetOwner());
}

void ULyraTeamSubsystem::FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const
{
    bIsPartOfTeam = false;
    TeamId = INDEX_NONE;
    if (UTeamSubsystem* Teams = GetWorld() ? GetWorld()->GetSubsystem<UTeamSubsystem>() : nullptr)
    {
        Teams->FindTeamFromActor(TestActor, bIsPartOfTeam, TeamId);
    }
}

ELyraTeamComparison ULyraTeamSubsystem::CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const
{
    if (UTeamSubsystem* Teams = GetWorld() ? GetWorld()->GetSubsystem<UTeamSubsystem>() : nullptr)
    {
        switch (Teams->CompareTeams(A, B, TeamIdA, TeamIdB))
        {
        case ETeamComparison::OnSameTeam: return ELyraTeamComparison::OnSameTeam;
        case ETeamComparison::DifferentTeams: return ELyraTeamComparison::DifferentTeams;
        default: break;
        }
    }
    TeamIdA = INDEX_NONE;
    TeamIdB = INDEX_NONE;
    return ELyraTeamComparison::InvalidArgument;
}

bool ULyraGamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
    if (UCoreGamePhaseSubsystem* Phases = GetWorld() ? GetWorld()->GetSubsystem<UCoreGamePhaseSubsystem>() : nullptr)
    {
        return Phases->IsPhaseActive(PhaseTag);
    }
    return false;
}

ALyraPlayerController* ULyraGameplayAbility_RangedWeapon::GetLyraPlayerControllerFromActorInfo() const
{
    return CurrentActorInfo ? Cast<ALyraPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr;
}

ALyraWeaponSpawner::ALyraWeaponSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
    RootComponent = CollisionVolume;
    PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
    PadMesh->SetupAttachment(CollisionVolume);
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(CollisionVolume);
}

void ALyraWeaponSpawner::PlayPickupEffects_Implementation()
{
}

void ALyraWeaponSpawner::PlayRespawnEffects_Implementation()
{
}

ALyraGameState::ALyraGameState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

UAbilitySystemComponent* ALyraGameState::GetLyraAbilitySystemComponent() const
{
    return GetCoreAbilitySystemComponent();
}

ALyraPlayerStart::ALyraPlayerStart(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

TArray<UActorComponent*> ULyraSystemStatics::FindComponentsByClass(
    AActor* TargetActor,
    TSubclassOf<UActorComponent> ComponentClass,
    bool bIncludeChildActors)
{
    TArray<UActorComponent*> Components;
    if (TargetActor != nullptr)
    {
        TargetActor->GetComponents(ComponentClass, Components, bIncludeChildActors);
    }
    return Components;
}

UAsyncAction_ObserveTeamColors* UAsyncAction_ObserveTeamColors::ObserveTeamColors(UObject* TeamAgent)
{
    if (TeamAgent == nullptr)
    {
        return nullptr;
    }

    UAsyncAction_ObserveTeamColors* Action = NewObject<UAsyncAction_ObserveTeamColors>();
    Action->TeamAgentObject = TeamAgent;
    Action->RegisterWithGameInstance(TeamAgent);
    return Action;
}

void UAsyncAction_ObserveTeamColors::Activate()
{
    UObject* TeamAgent = TeamAgentObject.Get();
    if (TeamAgent == nullptr)
    {
        SetReadyToDestroy();
        return;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(TeamAgent, EGetWorldErrorMode::LogAndReturnNull);
    UTeamSubsystem* TeamSubsystem = World ? World->GetSubsystem<UTeamSubsystem>() : nullptr;
    const int32 TeamId = TeamSubsystem ? TeamSubsystem->FindTeamFromObject(TeamAgent) : INDEX_NONE;
    const UTeamDisplayAsset* DisplayAsset = TeamSubsystem ? TeamSubsystem->GetEffectiveTeamDisplayAsset(TeamId, TeamAgent) : nullptr;

    if (ITeamAgentInterface* TeamInterface = Cast<ITeamAgentInterface>(TeamAgent))
    {
        TeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnWatchedAgentChangedTeam);
    }

    BroadcastChange(TeamId, DisplayAsset);
}

void UAsyncAction_ObserveTeamColors::SetReadyToDestroy()
{
    if (UObject* TeamAgent = TeamAgentObject.Get())
    {
        if (ITeamAgentInterface* TeamInterface = Cast<ITeamAgentInterface>(TeamAgent))
        {
            TeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
        }
    }

    Super::SetReadyToDestroy();
}

void UAsyncAction_ObserveTeamColors::BroadcastChange(int32 NewTeam, const UTeamDisplayAsset* DisplayAsset)
{
    UObject* TeamAgent = TeamAgentObject.Get();
    UWorld* World = TeamAgent ? GEngine->GetWorldFromContextObject(TeamAgent, EGetWorldErrorMode::ReturnNull) : nullptr;
    UTeamSubsystem* TeamSubsystem = World ? World->GetSubsystem<UTeamSubsystem>() : nullptr;

    if (TeamSubsystem && LastBroadcastTeamId != INDEX_NONE && LastBroadcastTeamId != NewTeam)
    {
        TeamSubsystem->GetTeamDisplayAssetChangedDelegate(LastBroadcastTeamId).RemoveAll(this);
    }

    LastBroadcastTeamId = NewTeam;
    OnTeamChanged.Broadcast(NewTeam != INDEX_NONE, NewTeam, DisplayAsset);

    if (TeamSubsystem && NewTeam != INDEX_NONE)
    {
        TeamSubsystem->GetTeamDisplayAssetChangedDelegate(NewTeam).AddDynamic(this, &ThisClass::OnDisplayAssetChanged);
    }
}

void UAsyncAction_ObserveTeamColors::OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
    UWorld* World = TeamAgent ? GEngine->GetWorldFromContextObject(TeamAgent, EGetWorldErrorMode::ReturnNull) : nullptr;
    UTeamSubsystem* TeamSubsystem = World ? World->GetSubsystem<UTeamSubsystem>() : nullptr;
    BroadcastChange(NewTeam, TeamSubsystem ? TeamSubsystem->GetEffectiveTeamDisplayAsset(NewTeam, TeamAgent) : nullptr);
}

void UAsyncAction_ObserveTeamColors::OnDisplayAssetChanged(const UTeamDisplayAsset* DisplayAsset)
{
    BroadcastChange(LastBroadcastTeamId, DisplayAsset);
}
