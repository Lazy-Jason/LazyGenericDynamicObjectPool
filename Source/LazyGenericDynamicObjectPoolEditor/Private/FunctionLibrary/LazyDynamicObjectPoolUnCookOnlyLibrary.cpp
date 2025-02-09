// // Copyright (C) 2024 Job Omondiale - All Rights Reserved


#include "FunctionLibrary\LazyDynamicObjectPoolUnCookOnlyLibrary.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_EnumLiteral.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"

DEFINE_LOG_CATEGORY(LogLazyDynamicObjectPoolEditor);

ULazyDynamicObjectPoolUnCookOnlyLibrary::ULazyDynamicObjectPoolUnCookOnlyLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{}

UEdGraphPin* ULazyDynamicObjectPoolUnCookOnlyLibrary::GenerateAssignmentNodesForPoolActor(FKismetCompilerContext& CompilerContext,
    UEdGraph* SourceGraph, const UK2Node_CallFunction* GetActorFromPoolNode, UEdGraphNode* SpawnPoolNode,
    UEdGraphPin* PoolActorPin, const UClass* ForClass)
{
    static const FName ObjectParamName(TEXT("Object"));
    static const FName ValueParamName(TEXT("Value"));
    static const FName PropertyNameParamName(TEXT("PropertyName"));

    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
    if (!Schema)
    {
        CompilerContext.MessageLog.Error(TEXT("Schema is null"));
        return nullptr;
    }

    UEdGraphPin* LastThen = GetActorFromPoolNode->GetThenPin();

    // Create 'set var by name' nodes and hook them up
    for (int32 PinIdx = 0; PinIdx < SpawnPoolNode->Pins.Num(); PinIdx++)
    {
        UEdGraphPin* OrgPin = SpawnPoolNode->Pins[PinIdx];
        const bool bHasDefaultValue = !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;

        if (!GetActorFromPoolNode->FindPin(OrgPin->PinName) && (OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue))
        {
            const FProperty* Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
            if (!Property) continue;

            if (OrgPin->LinkedTo.Num() == 0)
            {
                FString DefaultValueAsString;
                if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
                {
                    if (Schema->DoesDefaultValueMatch(*OrgPin, DefaultValueAsString)) continue;
                }
                else if (ForClass->ClassDefaultObject)
                {
                    FBlueprintEditorUtils::PropertyValueToString(Property, static_cast<const uint8*>(ForClass->ClassDefaultObject), DefaultValueAsString);
                    if (DefaultValueAsString == OrgPin->GetDefaultAsString()) continue;
                }
            }

            UK2Node_CallFunction* SetVarNode = nullptr;
            if (OrgPin->PinType.IsArray())
            {
                SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(SpawnPoolNode, SourceGraph);
            }
            else
            {
                SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnPoolNode, SourceGraph);
            }

            const UFunction* SetByNameFunction = Schema->FindSetVariableByNameFunction(OrgPin->PinType);
            if (!SetByNameFunction) continue;

            SetVarNode->SetFromFunction(SetByNameFunction);
            SetVarNode->AllocateDefaultPins();

            Schema->TryCreateConnection(LastThen, SetVarNode->GetExecPin());
            LastThen = SetVarNode->GetThenPin();

            UEdGraphPin* ObjectPin = SetVarNode->FindPinChecked(ObjectParamName);
            PoolActorPin->MakeLinkTo(ObjectPin);

            UEdGraphPin* PropertyNamePin = SetVarNode->FindPinChecked(PropertyNameParamName);
            PropertyNamePin->DefaultValue = OrgPin->PinName.ToString();

            UEdGraphPin* ValuePin = SetVarNode->FindPinChecked(ValueParamName);

            if (OrgPin->LinkedTo.Num() == 0 &&
            OrgPin->DefaultValue != FString() &&
            OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
            OrgPin->PinType.PinSubCategoryObject.IsValid() &&
            OrgPin->PinType.PinSubCategoryObject->IsA<UEnum>())
            {
                // Pin is an enum, we need to alias the enum value to an int:
                UK2Node_EnumLiteral* EnumLiteralNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnumLiteral>(SetVarNode, SourceGraph);
                EnumLiteralNode->Enum = CastChecked<UEnum>(OrgPin->PinType.PinSubCategoryObject.Get());
                EnumLiteralNode->AllocateDefaultPins();
                EnumLiteralNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(ValuePin);

                UEdGraphPin* InPin = EnumLiteralNode->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName());
                check( InPin );
                InPin->DefaultValue = OrgPin->DefaultValue;
            }
            else
            {
                // For non-array struct pins that are not linked, transfer the pin type so that the node will expand an auto-ref that will assign the value by-ref.
                if (OrgPin->PinType.IsArray() == false && OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && OrgPin->LinkedTo.Num() == 0)
                {
                    ValuePin->PinType.PinCategory = OrgPin->PinType.PinCategory;
                    ValuePin->PinType.PinSubCategory = OrgPin->PinType.PinSubCategory;
                    ValuePin->PinType.PinSubCategoryObject = OrgPin->PinType.PinSubCategoryObject;
                    CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
                }
                else
                {
                    // For interface pins we need to copy over the subcategory
                    if (OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface)
                    {
                        ValuePin->PinType.PinSubCategoryObject = OrgPin->PinType.PinSubCategoryObject;
                    }

                    CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
                    SetVarNode->PinConnectionListChanged(ValuePin);
                }
            }
            SetVarNode->PinConnectionListChanged(ValuePin);
        }
    }

    return LastThen;
}
