// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "LazyDynamicObjectPoolUnCookOnlyLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLazyDynamicObjectPoolEditor, Log, All);

/**
 * 
 */
UCLASS()
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API ULazyDynamicObjectPoolUnCookOnlyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static UEdGraphPin* GenerateAssignmentNodesForPoolActor( FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph,
	const UK2Node_CallFunction* GetActorFromPoolNode, UEdGraphNode* SpawnPoolNode, UEdGraphPin* PoolActorPin, const UClass* ForClass );
};
