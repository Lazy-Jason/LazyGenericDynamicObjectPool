// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SObjectPoolSearchAction;
class SObjectPoolQuickAction;
/**
 * 
 */
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API SObjectPoolDebugger : public SCompoundWidget
{
private:

	TSharedPtr<FTabManager> TabManager;
	TSharedPtr<SObjectPoolQuickAction> ObjectPoolQuickAction;
	TSharedPtr<SObjectPoolSearchAction> ObjectPoolSearchAction;
	
public:
	SLATE_BEGIN_ARGS(SObjectPoolDebugger)
	{}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
};
