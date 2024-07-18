// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "LazyDynamicObjectPoolLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLazyDynamicObjectPool, Log, All);

/**
 * 
 */
UCLASS()
class LAZYGENERICDYNAMICOBJECTPOOL_API ULazyDynamicObjectPoolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "LazyDynamicObjectPoolLibrary", meta = (WorldContext = "ContextObject", BlueprintInternalUseOnly = "true"))
	static ULazyDynamicObjectPoolSubsystem* GetSubsystem(const UObject* ContextObject);

	static UEdGraphPin* GenerateAssignmentNodesForPoolActor( FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph,
	const UK2Node_CallFunction* GetActorFromPoolNode, UEdGraphNode* SpawnPoolNode, UEdGraphPin* PoolActorPin, const UClass* ForClass );
};
