// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "DeveloperSettings/LazyDynamicObjectPoolSettings.h"
#include "Interface/PoolableActorInterface.h"

ULazyDynamicObjectPoolSubsystem::ULazyDynamicObjectPoolSubsystem()
{}

void ULazyDynamicObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    Settings = GetMutableDefault<ULazyDynamicObjectPoolSettings>();
    check(Settings);

    if (Settings->bEnableAutoShrink && IsValid(GetWorld()))
    {
        NextShrinkTime = Settings->AutoShrinkInterval;
        GetWorld()->GetTimerManager().SetTimer(ShrinkTimeProgressTimerHandle, this, &ULazyDynamicObjectPoolSubsystem::CalculateNextShrinkTime, 1, true);
        GetWorld()->GetTimerManager().SetTimer(AutoShrinkTimerHandle, this, &ULazyDynamicObjectPoolSubsystem::PerformAutoShrink, Settings->AutoShrinkInterval, true);
    }
}

void ULazyDynamicObjectPoolSubsystem::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(AutoShrinkTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(ShrinkTimeProgressTimerHandle);
    Super::Deinitialize();
}

bool ULazyDynamicObjectPoolSubsystem::CreatePool(TSubclassOf<AActor> ActorClass, int32 InitialSize)
{
    if (GetPoolSize(ActorClass) > 0) return false;

    const int32 PoolSize = (InitialSize > 0) ? InitialSize : Settings->DefaultInitialPoolSize;
    FObjectPool& NewPool = ObjectPools.Add(ActorClass, FObjectPool());
    GrowActorPool(NewPool, ActorClass, PoolSize);

    LogPoolOperation(FString::Printf(TEXT("Created actor pool for %s with size %d"), *ActorClass->GetName(), PoolSize), ActorClass);
    return true;
}

AActor* ULazyDynamicObjectPoolSubsystem::InitializeActorFromPool(const TSubclassOf<AActor> ActorClass, AActor* NewOwner)
{
    FObjectPool* Pool = ObjectPools.Find(ActorClass);
    if (!Pool)
    {
        CreatePool(ActorClass);
        Pool = ObjectPools.Find(ActorClass);
    }

    if (Pool->AvailableObjects.IsEmpty())
    {
        const int32 GrowthAmount = FMath::Max(1, FMath::FloorToInt(Pool->InUseObjects.Num() * (Settings->PoolGrowthFactor - 1.0f)));
        GrowActorPool(*Pool, ActorClass, GrowthAmount);
    }

    if (Pool->AvailableObjects.IsEmpty()) return nullptr;

    AActor* Actor = Cast<AActor>(Pool->AvailableObjects.Pop());
    if (!IsValid(Actor))
    {
        LogPoolOperation(FString::Printf(TEXT("Failed to initialize actor from pool for %s"), *ActorClass->GetName()), ActorClass);
        return nullptr;
    }

    Pool->AccessCount++;
    if(IsValid(NewOwner)) Actor->SetOwner(NewOwner);
    LogPoolOperation(FString::Printf(TEXT("Initialized actor from pool for %s"), *ActorClass->GetName()), ActorClass);
    return Actor;
}

AActor* ULazyDynamicObjectPoolSubsystem::FinishInitializeActorFromPool(AActor* Actor, const FTransform& NewTransform, bool bSweep, 
FHitResult OutSweepHitResult, ETeleportType Teleport)
{
    if (!IsValid(Actor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to activate invalid actor from pool"));
        return nullptr;
    }

    const TSubclassOf<AActor> ActorClass = Actor->GetClass();
    FObjectPool* Pool = ObjectPools.Find(ActorClass);
    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to activate actor of type %s from non-existent pool"), *ActorClass->GetName());
        return Actor;
    }

    Pool->InUseObjects.Add(Actor);

    Actor->SetActorTransform(NewTransform, bSweep, &OutSweepHitResult, Teleport);
    // Activate the actor and its components
    ActivateActor(Actor);

    LogPoolOperation(FString::Printf(TEXT("Activated actor from pool for %s"), *ActorClass->GetName()), ActorClass);
    return Actor;
}

void ULazyDynamicObjectPoolSubsystem::ReturnActorToPool(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    const TSubclassOf<AActor> ActorClass = Actor->GetClass();
    FObjectPool* Pool = ObjectPools.Find(ActorClass);
    if (!Pool)
    {
        LogPoolOperation(FString::Printf(TEXT("Attempted to return actor of type %s to non-existent pool"), *ActorClass->GetName()), ActorClass);
        return;
    }

    // Deactivate the actor and its components
    DeactivateActor(Actor);

    Pool->InUseObjects.Remove(Actor);
    Pool->AvailableObjects.Add(Actor);

    LogPoolOperation(FString::Printf(TEXT("Returned actor to pool for %s"), *ActorClass->GetName()), ActorClass);
}

int32 ULazyDynamicObjectPoolSubsystem::GetPoolSize(const TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    return Pool ? Pool->AvailableObjects.Num() + Pool->InUseObjects.Num() : 0;
}

void ULazyDynamicObjectPoolSubsystem::ActivateActor(AActor* Actor)
{
    if (!IsValid(Actor))return;

    Actor->SetActorHiddenInGame(false);
    Actor->SetActorEnableCollision(true);
    Actor->SetActorTickEnabled(true);

    // Call a custom reset function if the actor implements it
    if (Actor->Implements<UPoolableActorInterface>())
    {
        IPoolableActorInterface::Execute_OnActivateFromPool(Actor);
        return;
    }

    // Activate and reset components
    TInlineComponentArray<UActorComponent*> Components;
    Actor->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        if (!IsValid(Component)) continue;

        // Activate the component for further use
        Component->Activate(true);
    }
}

void ULazyDynamicObjectPoolSubsystem::DeactivateActor(AActor* Actor)
{
    if (!IsValid(Actor))return;

    Actor->SetActorHiddenInGame(true);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorTickEnabled(false);

    // Call a custom reset function if the actor implements it
    if (Actor->Implements<UPoolableActorInterface>())
    {
        IPoolableActorInterface::Execute_OnDeactivateToPool(Actor);
        return;
    }
    

    // Deactivate components
    TInlineComponentArray<UActorComponent*> Components;
    Actor->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        if (!IsValid(Component)) continue;

        // Deactivate the component for further use
        Component->Deactivate();
    }
}

void ULazyDynamicObjectPoolSubsystem::GrowActorPool(FObjectPool& Pool, TSubclassOf<AActor> ActorClass, int32 GrowthAmount)
{
    const int32 CurrentSize = Pool.AvailableObjects.Num() + Pool.InUseObjects.Num();
    const int32 MaxGrowth = Settings->MaxPoolSize > 0 ? FMath::Min(GrowthAmount, Settings->MaxPoolSize - CurrentSize) : GrowthAmount;

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to grow actor pool: World is null"));
        return;
    }

    for (int32 i = 0; i < MaxGrowth; ++i)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (!IsValid(NewActor)) return;
        OnActorSpawn.Broadcast();

        // actors are not suppose to be destroyed manually by the actor instead to be released to the pool,
        // how ever in the case it does happen we want to handle that case.
        NewActor->OnDestroyed.AddDynamic(this, &ULazyDynamicObjectPoolSubsystem::HandleActorDestroyed);
        DeactivateActor(NewActor);
        Pool.AvailableObjects.Add(NewActor);
        Pool.TotalGrowthOperations ++;
    }

    LogPoolOperation(FString::Printf(TEXT("Grew actor pool for %s by %d actors"), *ActorClass->GetName(), MaxGrowth), ActorClass);
}

void ULazyDynamicObjectPoolSubsystem::ShrinkPool(FObjectPool& Pool)
{
    // Calculate the total size and target size
    const int32 TotalSize = Pool.AvailableObjects.Num() + Pool.InUseObjects.Num();
    const int32 TargetSize = FMath::Max(Settings->DefaultInitialPoolSize, FMath::CeilToInt(TotalSize * (1.0f - Settings->ShrinkThreshold)));

    // Check if shrinking is necessary
    const int32 NumToRemove = FMath::Max((Pool.AvailableObjects.Num() - TargetSize), 0);
    if (NumToRemove <= 0) return;

    const UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to shrink actor pool: World is null"));
        return;
    }

    // Remove actors
    int32 ActuallyRemoved = 0;
    for (int32 i = 0; i < NumToRemove; ++i)
    {
        if (Pool.AvailableObjects.Num() == 0) break;

        AActor* ActorToRemove = Pool.AvailableObjects.Pop(false);
        if (ensure(ActorToRemove))
        {
            ActorToRemove->Destroy();
            ++ActuallyRemoved;
            OnActorDestroy.Broadcast();
        }
    }

    // Shrink the array to avoid unnecessary memory usage
    Pool.AvailableObjects.Shrink();

    // Update stats
    TotalShrinkOperations++;

    // Log the operation
    LogPoolOperation(FString::Printf(TEXT("Shrunk actor pool by %d actors (attempted %d)"), ActuallyRemoved, NumToRemove), AActor::StaticClass());
}

void ULazyDynamicObjectPoolSubsystem::PerformAutoShrink()
{
    for (auto& Pair : ObjectPools)
    {
        ShrinkPool(Pair.Value);
        LogPoolOperation(FString::Printf(TEXT("Auto-shrunk pool for %s"), *Pair.Key->GetName()), Pair.Key);
    }
}

void ULazyDynamicObjectPoolSubsystem::HandleActorDestroyed(AActor* DestroyedActor)
{
    // Handles the case where an actor is destroyed instead of returned to the pool
    const TSubclassOf<AActor> ActorClass = DestroyedActor->GetClass();
    FObjectPool* Pool = ObjectPools.Find(ActorClass);
    
    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to handle destruction of actor of type %s not managed by any pool"), *ActorClass->GetName());
        return;
    }

    Pool->InUseObjects.Remove(DestroyedActor);
    Pool->AvailableObjects.Remove(DestroyedActor);
    OnActorDestroy.Broadcast();

    LogPoolOperation(FString::Printf(TEXT("Handled destruction of actor from pool for %s"), *ActorClass->GetName()), ActorClass);
}

void ULazyDynamicObjectPoolSubsystem::LogPoolOperation(const FString& Operation, const TSubclassOf<AActor> ClassType) const
{
    if (Settings->bEnableDetailedLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("%s - Class: %s"), *Operation, *ClassType->GetName());
    }
}

void ULazyDynamicObjectPoolSubsystem::CalculateNextShrinkTime()
{
    NextShrinkTime -= 1;
    if(NextShrinkTime <= 0)
    {
        NextShrinkTime = Settings->AutoShrinkInterval;
    }
}

TArray<AActor*> ULazyDynamicObjectPoolSubsystem::GetAvailableActorsInPool(TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    return Pool ? Pool->AvailableObjects : TArray<AActor*>();
}

TArray<AActor*> ULazyDynamicObjectPoolSubsystem::GetInUseActorsInPool(TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    return Pool ? Pool->InUseObjects : TArray<AActor*>();
}

TArray<AActor*> ULazyDynamicObjectPoolSubsystem::GetAllActorsInPool(TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    if (!Pool) return TArray<AActor*>();

    TArray<AActor*> AllActors = Pool->AvailableObjects;
    AllActors.Append(Pool->InUseObjects);
    return AllActors;
}

TArray<TSubclassOf<AActor>> ULazyDynamicObjectPoolSubsystem::GetAllPooledClasses() const
{
    TArray<TSubclassOf<AActor>> Classes;
    ObjectPools.GetKeys(Classes);
    return Classes;
}

int32 ULazyDynamicObjectPoolSubsystem::GetTotalActorsInAllPools() const
{
    int32 TotalActors = 0;
    for (const auto& Pair : ObjectPools)
    {
        TotalActors += Pair.Value.AvailableObjects.Num() + Pair.Value.InUseObjects.Num();
    }
    return TotalActors;
}

int32 ULazyDynamicObjectPoolSubsystem::GetMaximumPoolSize() const
{
    return Settings ? Settings->MaxPoolSize : 0;
}

int32 ULazyDynamicObjectPoolSubsystem::GetPoolAccessCount(TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    return Pool ? Pool->AccessCount : 0;
}

int32 ULazyDynamicObjectPoolSubsystem::GetPoolGrowthOperation(TSubclassOf<AActor> ClassType) const
{
    const FObjectPool* Pool = ObjectPools.Find(ClassType);
    return Pool ? Pool->TotalGrowthOperations : 0;
}

void ULazyDynamicObjectPoolSubsystem::ClearAllPools()
{
    for (auto& Pair : ObjectPools)
    {
        FObjectPool& Pool = Pair.Value;
        for (AActor* Actor : Pool.AvailableObjects)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        for (AActor* Actor : Pool.InUseObjects)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.AvailableObjects.Empty();
        Pool.InUseObjects.Empty();
        Pool.AccessCount = 0;
        Pool.TotalGrowthOperations = 0;
    }
    ObjectPools.Empty();
    LogPoolOperation(TEXT("Cleared all pools"), AActor::StaticClass());
}

void ULazyDynamicObjectPoolSubsystem::ShrinkAllPools()
{
    for (auto& Pair : ObjectPools)
    {
        ShrinkPool(Pair.Value);
    }
    LogPoolOperation(TEXT("Shrunk all pools"), AActor::StaticClass());
}

float ULazyDynamicObjectPoolSubsystem::GetTotalActorsInPoolRatio()
{
    return static_cast<float>(GetTotalActorsInAllPools()) / static_cast<float>(GetMaximumPoolSize());
}
