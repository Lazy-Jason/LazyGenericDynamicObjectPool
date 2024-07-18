// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LazyDynamicObjectPoolSubsystem.generated.h"

class ULazyDynamicObjectPoolSettings;

DECLARE_MULTICAST_DELEGATE(FDynamicObjectPoolAction);

UCLASS()
class LAZYGENERICDYNAMICOBJECTPOOL_API ULazyDynamicObjectPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    FDynamicObjectPoolAction OnActorAddedToPool;
    FDynamicObjectPoolAction OnActorRemovedFromPool;
    FDynamicObjectPoolAction OnActorSpawn;
    FDynamicObjectPoolAction OnActorDestroy;


private:
 
    struct FObjectPool
    {
        TArray<AActor*> AvailableObjects;
        TArray<AActor*> InUseObjects;
        int32 AccessCount = 0;
    };

    TMap<TSubclassOf<AActor>, FObjectPool> ObjectPools;

    UPROPERTY()
    ULazyDynamicObjectPoolSettings* Settings = nullptr;

    FTimerHandle AutoShrinkTimerHandle;
    FTimerHandle ShrinkTimeProgressTimerHandle;
    float NextShrinkTime = 0;
    int32 TotalShrinkOperations = 0;

public:
    ULazyDynamicObjectPoolSubsystem();
 
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    bool CreatePool(TSubclassOf<AActor> ClassType, int32 InitialSize = -1);

    UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (BlueprintInternalUseOnly = "true"))
    AActor* InitializeActorFromPool(TSubclassOf<AActor> ActorClass, AActor* NewOwner);

    UFUNCTION(BlueprintCallable, Category = "Object Pool", meta = (BlueprintInternalUseOnly = "true"))
    AActor* FinishInitializeActorFromPool(AActor* Actor, const FTransform& NewTransform, bool bSweep = false,
    FHitResult OutSweepHitResult = FHitResult(), ETeleportType Teleport = ETeleportType::None);

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    void ReturnActorToPool(AActor* Actor);
 
    UFUNCTION(BlueprintPure, Category = "Object Pool")
    int32 GetPoolSize(TSubclassOf<AActor> ClassType) const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    float GetNextAutoShrinkTime() const { return NextShrinkTime; }

    // New helper functions
    UFUNCTION(BlueprintPure, Category = "Object Pool")
    TArray<AActor*> GetAvailableActorsInPool(TSubclassOf<AActor> ClassType) const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    TArray<AActor*> GetInUseActorsInPool(TSubclassOf<AActor> ClassType) const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    TArray<AActor*> GetAllActorsInPool(TSubclassOf<AActor> ClassType) const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    TArray<TSubclassOf<AActor>> GetAllPooledClasses() const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    int32 GetTotalActorsInAllPools() const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    int32 GetMaximumPoolSize() const;

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    int32 GetTotalShrinkOperations() const { return TotalShrinkOperations; }

    UFUNCTION(BlueprintPure, Category = "Object Pool")
    int32 GetPoolAccessCount(TSubclassOf<AActor> ClassType) const;

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    void ClearAllPools();

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    void ShrinkAllPools();

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    float GetTotalActorsInPoolRatio();

private:
    void ActivateActor(AActor* Actor);
    void DeactivateActor(AActor* Actor);
    void GrowActorPool(FObjectPool& Pool, TSubclassOf<AActor> ActorClass, int32 GrowthAmount);
    void ShrinkPool(FObjectPool& Pool);
    void PerformAutoShrink();
    
    UFUNCTION()
    void HandleActorDestroyed(AActor* DestroyedActor);
    
    void LogPoolOperation(const FString& Operation, TSubclassOf<AActor> ClassType) const;
    void CalculateNextShrinkTime();
};