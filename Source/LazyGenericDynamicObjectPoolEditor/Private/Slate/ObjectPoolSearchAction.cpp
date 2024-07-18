// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "Slate/ObjectPoolSearchAction.h"

#include "EditorClassUtils.h"
#include "SlateOptMacros.h"
#include "Commandlets/GatherTextCommandlet.h"
#include "Engine/StaticMeshActor.h"
#include "SceneOutliner/Public/SOutlinerTreeView.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "Engine/World.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"

void SObjectPoolSearchAction::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("ActorPoolLabel", "Actor Pool"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
            ]
            + SHorizontalBox::Slot()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(this, reinterpret_cast<TAttribute<FText>::FGetter::TConstMethodPtr<SObjectPoolSearchAction>>(&
                          SObjectPoolSearchAction::GetActorTypeText))
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SSearchBox)
            .HintText(LOCTEXT("SearchBoxHint", "Search actors..."))
            .OnTextChanged(this, &SObjectPoolSearchAction::OnSearchTextChanged)
        ]
        + SVerticalBox::Slot()
        .FillHeight(1.f)
        [
            SNew(SBox)
            .HAlign(HAlign_Fill)
            .MaxDesiredHeight(300)
            [
                SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SAssignNew(ActorListView, SListView<TSharedPtr<FPooledActor>>)
                    .ItemHeight(25)
                    .ListItemsSource(&FilteredActors)
                    .OnGenerateRow(this, &SObjectPoolSearchAction::OnGenerateRowForList)
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(STextBlock)
            .Text(this, reinterpret_cast<TAttribute<FText>::FGetter::TConstMethodPtr<SObjectPoolSearchAction>>(&
                      SObjectPoolSearchAction::GetFilterActorStatusText))
        ]
    ];
}

void SObjectPoolSearchAction::RegisterPoolDelegate()
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return;
    
    ULazyDynamicObjectPoolSubsystem* Subsystem = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>();
    if(!Subsystem) return;
        
    OnActorSpawnHandle = Subsystem->OnActorSpawn.AddLambda([this]() { RefreshActorList(); });
    OnActorDestroyHandle = Subsystem->OnActorDestroy.AddLambda([this]() { RefreshActorList(); });
}

void SObjectPoolSearchAction::UnregisterPoolDelegate() const
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return;
    
    ULazyDynamicObjectPoolSubsystem* Subsystem = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>();
    if(!Subsystem) return;
        
    Subsystem->OnActorSpawn.Remove(OnActorSpawnHandle);
    Subsystem->OnActorDestroy.Remove(OnActorDestroyHandle);
}

void SObjectPoolSearchAction::RefreshActorList()
{
    AllActors.Empty();
    FilteredActors.Empty();

    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return;

    TArray<TSubclassOf<AActor>> PooledClasses = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetAllPooledClasses();
    for (const TSubclassOf<AActor> Class : PooledClasses)
    {
        TArray<AActor*> Actors = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetAllActorsInPool(Class);
        for (AActor* Actor : Actors)
        {
            AllActors.Add(MakeShared<FPooledActor>(Actor));
        }
    }

    FilteredActors = AllActors;
    ActorListView->RequestListRefresh();
}

TSharedRef<ITableRow> SObjectPoolSearchAction::OnGenerateRowForList(TSharedPtr<FPooledActor> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<FPooledActor>>, OwnerTable)
    .Padding(FMargin(4))
    [
        SNew(SHorizontalBox)
        
        // Icon
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(0, 0, 4, 0)
        [
            SNew(SImage)
            .Image(FAppStyle::GetBrush(Item->Actor->IsA<AStaticMeshActor>() ? "ClassIcon.StaticMeshActor" : "ClassIcon.Actor"))
            .ColorAndOpacity(FSlateColor::UseForeground())
        ]
        
        // Actor Name
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .VAlign(VAlign_Center)
        .Padding(0, 0, 8, 0)
        [
            SNew(STextBlock)
            .Text(FText::FromString(Item->Actor->GetName()))
            .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
            .ColorAndOpacity(FSlateColor::UseForeground())
        ]
        
        // Actor Class
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            FEditorClassUtils::GetSourceLink(Item->Actor->GetClass(), FEditorClassUtils::FSourceLinkParams().Object = Item->Actor)
        ]
    ];
}

void SObjectPoolSearchAction::OnSearchTextChanged(const FText& InSearchText)
{
    FilteredActors.Empty();

    const FString FilterString = InSearchText.ToString();
    if (FilterString.IsEmpty())
    {
        FilteredActors = AllActors;
    }
    else
    {
        for (const TSharedPtr<FPooledActor>& Actor : AllActors)
        {
            if (Actor->Actor->GetName().Contains(FilterString))
            {
                FilteredActors.Add(Actor);
            }
        }
    }

    ActorListView->RequestListRefresh();
}

FText SObjectPoolSearchAction::GetFilterActorStatusText()
{
    UnregisterPoolDelegate();
    RegisterPoolDelegate();
    RefreshActorList();
    return FText::Format(LOCTEXT("FilterTextBlock", "Number of actors: {0}"), FText::AsNumber(FilteredActors.Num()));
}

FText SObjectPoolSearchAction::GetActorTypeText()
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText::FromString("N/A");
    
    UnregisterPoolDelegate();
    RegisterPoolDelegate();
    RefreshActorList();
    
    if (AllActors.Num() > 0)
    {
        return FText::FromString(AllActors[0]->Actor->GetClass()->GetName());
    }
    return LOCTEXT("NoActorsText", "No actors in pool");
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE