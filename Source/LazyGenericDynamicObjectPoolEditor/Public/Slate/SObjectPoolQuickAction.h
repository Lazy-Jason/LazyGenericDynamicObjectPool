// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API SObjectPoolQuickAction : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SObjectPoolQuickAction)
	{} // constructor

	SLATE_ATTRIBUTE(const TSubclassOf<AActor>*, SelectedActorClass)
	
	SLATE_END_ARGS()

	void OnActorClassSelected(const UClass* Class);
	UClass* GetSelectedActorClass();
	FText GetNextPoolShrinkTime() const;
	FText GetLastAccessTime() const;
	FText GetTotalActorText() const;
	FText GetMaxPoolSizeText() const;
	FText GetShrinkOperationsText() const;
	FText GetActorsOfClassInUse() const;
	FText GetActorsOfClassInPool() const;
	TOptional<float> GetTotalPoolSizeRatio() const;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	FReply OnClearAllPoolClicked();
	FReply OnShrinkAllPoolClicked();

private:
	TSubclassOf<AActor> SelectedClass;
};
