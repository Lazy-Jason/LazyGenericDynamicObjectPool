// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "LazyDynamicObjectPoolLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLazyDynamicObjectPool, Log, All);

/**
 * 
 */
UCLASS()
class LAZYGENERICDYNAMICOBJECTPOOL_API ULazyDynamicObjectPoolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "LazyDynamicObjectPoolLibrary", meta = (WorldContext = "ContextObject", BlueprintInternalUseOnly = "true"))
	static ULazyDynamicObjectPoolSubsystem* GetSubsystem(const UObject* ContextObject);
};
