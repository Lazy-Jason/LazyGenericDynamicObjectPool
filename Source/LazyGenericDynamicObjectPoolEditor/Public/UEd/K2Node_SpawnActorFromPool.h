// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "K2Node_SpawnActorFromPool.generated.h"

/**
 * 
 */
UCLASS()
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API UK2Node_SpawnActorFromPool : public UK2Node_ConstructObjectFromClass
{
	GENERATED_BODY()

	//Start of UEdGraphNode interface implementation
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* Graph) const override;
	//End of implementation

	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void GetNodeAttributes( TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes ) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	//~ End UK2Node Interface
	
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

	//~ Begin UK2Node_ConstructObjectFromClass Interface
	virtual UClass* GetClassPinBaseClass() const override;
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) const override;
	UK2Node_CallFunction* CreateGetSubsystemNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph,
	                                             UEdGraphPin*& OutWorldContextPin);
	//~ End UK2Node_ConstructObjectFromClass Interface

private:
	UEdGraphPin* GetActorTransformPin() const;
	UEdGraphPin* GetOwnerPin() const;
};
