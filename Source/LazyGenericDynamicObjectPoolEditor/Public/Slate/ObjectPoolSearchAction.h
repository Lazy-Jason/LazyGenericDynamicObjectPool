// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/SCompoundWidget.h"

class ULazyDynamicObjectPoolSubsystem;

struct FPooledActor
{
	TWeakObjectPtr<AActor> Actor;
	FPooledActor(AActor* InActor) : Actor(InActor) {}
};

/**
 * 
 */
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API SObjectPoolSearchAction : public SCompoundWidget
{
	TSharedPtr<SListView<TSharedPtr<FPooledActor>>> ActorListView;
	TArray<TSharedPtr<FPooledActor>> AllActors;
	TArray<TSharedPtr<FPooledActor>> FilteredActors;

	FDelegateHandle OnActorSpawnHandle;
	FDelegateHandle OnActorDestroyHandle;
	
public:
	SLATE_BEGIN_ARGS(SObjectPoolSearchAction)
	{}

	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	void RefreshActorList();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FPooledActor> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSearchTextChanged(const FText& InSearchText);
	FText GetFilterActorStatusText();
	FText GetActorTypeText();

	void RegisterPoolDelegate();
	void UnregisterPoolDelegate() const;
};
