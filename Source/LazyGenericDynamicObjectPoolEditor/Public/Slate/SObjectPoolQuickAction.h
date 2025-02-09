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

	float TopLevelContentPadding = 12.0f;
	float MidLevelContentPadding = 12.0f;
	float BottomLevelContentPadding = 12.0f;

	FSlateColor ButtonColor = FSlateColor(FColor::Transparent);

	void OnActorClassSelected(const UClass* Class);
	UClass* GetSelectedActorClass();
	FText GetNextPoolShrinkTime() const;
	FText GetLastAccessTime() const;
	FText GetTotalActorText() const;
	FText GetMaxPoolSizeText() const;
	FText GetShrinkOperationsText() const;
	FText GetActorsOfClassInUse() const;
	FText GetActorsOfClassInPool() const;
	FText GetPoolGrowthOperation() const;
	FSlateColor GetButtonColor() const { return ButtonColor; };
	void SetButtonHoverColor();
	void ResetButtonColor();
	TOptional<float> GetTotalPoolSizeRatio() const;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	FReply OnClearAllPoolClicked();
	FReply OnShrinkAllPoolClicked();

private:
	TSubclassOf<AActor> SelectedClass;
};
