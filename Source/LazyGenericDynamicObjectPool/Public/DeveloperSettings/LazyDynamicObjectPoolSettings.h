// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LazyDynamicObjectPoolSettings.generated.h"

/**
 * @class ULazyDynamicObjectPoolSettings
 * @brief Configuration settings for the Lazy Dynamic Object Pool system.
 *
 * This class defines the configurable parameters for the Lazy Dynamic Object Pool plugin.
 * It allows developers to fine-tune the behavior of the object pool system through the
 * Project Settings in the Unreal Editor.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Lazy Dynamic Object Pool"))
class LAZYGENERICDYNAMICOBJECTPOOL_API ULazyDynamicObjectPoolSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /** @brief Default constructor */
    ULazyDynamicObjectPoolSettings(const FObjectInitializer& ObjectInitializer);

    /**
     * @brief The default initial size for new object pools.
     * @note This setting determines how many objects are pre-instantiated when a new pool is created.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Configuration", meta = (ClampMin = "0", UIMin = "0"))
    int32 DefaultInitialPoolSize = 10;

    /**
     * @brief The maximum size limit for object pools.
     * @note Pools will not grow beyond this size. Set to 0 for unlimited growth.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Configuration", meta = (ClampMin = "0", UIMin = "0"))
    int32 MaxPoolSize = 1000;

    /**
     * @brief The growth factor for pools when they need to expand.
     * @note When a pool needs more objects, it will grow by this percentage of its current size.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Configuration", meta = (ClampMin = "1.0", UIMin = "1.0", ClampMax = "2.0", UIMax = "2.0"))
    float PoolGrowthFactor = 1.5f;

    /**
     * @brief Whether to enable automatic pool shrinking.
     * @note If true, pools will periodically remove excess unused objects.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Optimization")
    bool bEnableAutoShrink = true;

    /**
     * @brief The interval in seconds between automatic pool shrink operations.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Optimization", meta = (EditCondition = "bEnableAutoShrink", ClampMin = "1.0", UIMin = "1.0"))
    float AutoShrinkInterval = 60.0f;

    /**
     * @brief The threshold of unused objects (as a percentage of pool size) that triggers shrinking.
     */
    UPROPERTY(config, EditAnywhere, Category = "Pool Optimization", meta = (EditCondition = "bEnableAutoShrink", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
    float ShrinkThreshold = 0.25f;

    /**
     * @brief Whether to log detailed information about pool operations.
     */
    UPROPERTY(config, EditAnywhere, Category = "Debugging")
    bool bEnableDetailedLogging = false;
};