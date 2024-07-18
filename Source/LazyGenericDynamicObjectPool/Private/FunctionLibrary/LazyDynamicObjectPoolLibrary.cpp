// // Copyright (C) 2024 Job Omondiale - All Rights Reserved


#include "FunctionLibrary/LazyDynamicObjectPoolLibrary.h"

#include "BlueprintCompilationManager.h"
#include "KismetCompiler.h"
#include "Interface/PoolableActorInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"

DEFINE_LOG_CATEGORY(LogLazyDynamicObjectPool);

ULazyDynamicObjectPoolLibrary::ULazyDynamicObjectPoolLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{}

ULazyDynamicObjectPoolSubsystem* ULazyDynamicObjectPoolLibrary::GetSubsystem(const UObject* ContextObject)
{
	if(!IsValid(ContextObject))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: ContextObject is not valid"));
		return nullptr;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: World is not valid"));
		return nullptr;
	}

	ULazyDynamicObjectPoolSubsystem* Subsystem = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>();
	if(!IsValid(Subsystem))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: ULazyDynamicObjectPoolSubsystem is not valid"));
	}
	else
	{
		UE_LOG(LogLazyDynamicObjectPool, Log, TEXT("GetSubsystem: Successfully retrieved ULazyDynamicObjectPoolSubsystem"));
	}

	return Subsystem;
}

UEdGraphPin* ULazyDynamicObjectPoolLibrary::GenerateAssignmentNodesForPoolActor(FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph, const UK2Node_CallFunction* GetActorFromPoolNode, UEdGraphNode* SpawnPoolNode,
	UEdGraphPin* PoolActorPin, const UClass* ForClass)
{
	static const FName ObjectParamName(TEXT("Object"));
    static const FName ValueParamName(TEXT("Value"));
    static const FName PropertyNameParamName(TEXT("PropertyName"));

    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
    UEdGraphPin* LastThen = GetActorFromPoolNode->GetThenPin();

    // Create 'set var by name' nodes and hook them up
    for (int32 PinIdx = 0; PinIdx < SpawnPoolNode->Pins.Num(); PinIdx++)
    {
        // Only create 'set param by name' node if this pin is linked to something
        UEdGraphPin* OrgPin = SpawnPoolNode->Pins[PinIdx];
        const bool bHasDefaultValue = !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;
        if (nullptr == GetActorFromPoolNode->FindPin(OrgPin->PinName) &&
            (OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue))
        {
	        const FProperty* Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
            // NULL property indicates that this pin was part of the original node, not the 
            // class we're assigning to:
            if (!Property)
            {
                continue;
            }

            if (OrgPin->LinkedTo.Num() == 0)
            {
                // We don't want to generate an assignment node unless the default value 
                // differs from the value in the CDO:
                FString DefaultValueAsString;

                if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
                {
                    if (Schema->DoesDefaultValueMatch(*OrgPin, DefaultValueAsString))
                    {
                        continue;
                    }
                }
                else if (ForClass->ClassDefaultObject)
                {
                    FBlueprintEditorUtils::PropertyValueToString(Property, (uint8*)ForClass->ClassDefaultObject, DefaultValueAsString);

                    if (DefaultValueAsString == OrgPin->GetDefaultAsString())
                    {
                        continue;
                    }
                }
            }

            UK2Node_CallFunction* SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnPoolNode, SourceGraph);
            const UFunction* SetByNameFunction = Schema->FindSetVariableByNameFunction(OrgPin->PinType);
            SetVarNode->SetFromFunction(SetByNameFunction);
            SetVarNode->AllocateDefaultPins();

            // Connect this node into the exec chain
            Schema->TryCreateConnection(LastThen, SetVarNode->GetExecPin());
            LastThen = SetVarNode->GetThenPin();

            // Connect the new object to the 'object' pin
            UEdGraphPin* ObjectPin = SetVarNode->FindPinChecked(ObjectParamName);
            PoolActorPin->MakeLinkTo(ObjectPin);

            // Fill in literal for 'property name' pin - name of pin is property name
            UEdGraphPin* PropertyNamePin = SetVarNode->FindPinChecked(PropertyNameParamName);
            PropertyNamePin->DefaultValue = OrgPin->PinName.ToString();

            UEdGraphPin* ValuePin = SetVarNode->FindPinChecked(ValueParamName);
            
            // Handle different property types
            if (Property->IsA<FBoolProperty>())
            {
                ValuePin->DefaultValue = OrgPin->DefaultValue.ToBool() ? TEXT("true") : TEXT("false");
            }
            else if (Property->IsA<FFloatProperty>() || Property->IsA<FDoubleProperty>())
            {
                ValuePin->DefaultValue = OrgPin->DefaultValue;
            }
            else if (Property->IsA<FIntProperty>())
            {
                ValuePin->DefaultValue = FString::FromInt(FCString::Atoi(*OrgPin->DefaultValue));
            }
            else if (Property->IsA<FNameProperty>())
            {
                ValuePin->DefaultValue = OrgPin->DefaultValue;
            }
            else if (Property->IsA<FStrProperty>())
            {
                ValuePin->DefaultValue = OrgPin->DefaultValue;
            }
            else
            {
                // For other types, move the connection
                CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
            }

            SetVarNode->PinConnectionListChanged(ValuePin);
        }
    }

    return LastThen;
}
