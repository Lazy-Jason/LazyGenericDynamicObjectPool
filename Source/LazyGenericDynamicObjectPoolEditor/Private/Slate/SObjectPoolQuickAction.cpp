// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "Slate/SObjectPoolQuickAction.h"

#include "FLazyGenericDynamicObjectPoolStyle.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "PropertyCustomizationHelpers.h"
#include "Slate/ObjectPoolActionButton.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "Widgets/Images/SLayeredImage.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SObjectPoolQuickAction::Construct(const FArguments& InArgs)
{
    const FLinearColor DarkBackgroundColor = FLinearColor(FColor(190, 190, 190));
    const FLinearColor LightBackgroundColor = FLinearColor(FColor(230, 230, 230));
    constexpr FLinearColor LightTextColor = FLinearColor(0.431f, 0.431f, 0.431f);
    
    ChildSlot
    [
        SNew(SBorder)
        .Padding(10)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
        .BorderBackgroundColor(DarkBackgroundColor)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                // Actor Class Selector
                + SHorizontalBox::Slot()
                .FillWidth(0.5f)
                .Padding(FMargin(5,0,5,0))
                [
                    SNew(SBorder)
                    .Padding(FMargin(TopLevelContentPadding))
                    .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
                    .BorderBackgroundColor(LightBackgroundColor)
                    [
                        SNew(SClassPropertyEntryBox)
                        .MetaClass(AActor::StaticClass())
                        .SelectedClass(this, reinterpret_cast<TAttribute<const UClass*>::FGetter::TConstMethodPtr<SObjectPoolQuickAction>>(&
                        SObjectPoolQuickAction::GetSelectedActorClass))
                        .OnSetClass(this, &SObjectPoolQuickAction::OnActorClassSelected)
                    ]
                ]
                // Actors In Use
                + SHorizontalBox::Slot()
                .FillWidth(0.16f)
                .Padding(FMargin(5,0,5,0))
                [
                    SNew(SBorder)
                    .Padding(FMargin(TopLevelContentPadding))
                    .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
                    .BorderBackgroundColor(LightBackgroundColor)
                    [
                        SNew(STextBlock)
                        .AutoWrapText(true)
                        .Justification(ETextJustify::Center)
                        .Text(this, &SObjectPoolQuickAction::GetActorsOfClassInUse)
                        .ColorAndOpacity(LightTextColor)
                    ]
                ]
                // Available Actors
                + SHorizontalBox::Slot()
                .FillWidth(0.16f)
                .Padding(FMargin(5,0,5,0))
                [
                    SNew(SBorder)
                    .Padding(FMargin(TopLevelContentPadding))
                    .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
                    .BorderBackgroundColor(LightBackgroundColor)
                    [
                        SNew(STextBlock)
                        .AutoWrapText(true)
                        .Justification(ETextJustify::Center)
                        .Text(this, &SObjectPoolQuickAction::GetActorsOfClassInPool)
                        .ColorAndOpacity(LightTextColor)
                    ]
                ]
                // Growth Rotation
                + SHorizontalBox::Slot()
                .FillWidth(0.16f)
                .Padding(FMargin(5,0,5,0))
                [
                    SNew(SBorder)
                    .Padding(FMargin(TopLevelContentPadding))
                    .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
                    .BorderBackgroundColor(LightBackgroundColor)
                    [
                        SNew(STextBlock)
                        .AutoWrapText(true)
                        .Justification(ETextJustify::Center)
                        .Text(this, &SObjectPoolQuickAction::GetPoolGrowthOperation)
                        .ColorAndOpacity(LightTextColor)
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.f)
            [
                SNew(SHorizontalBox)
                    
                // Action Buttons
                + SHorizontalBox::Slot()
                .FillWidth(0.7f)
                [
                    SNew(SUniformWrapPanel)
                    .SlotPadding(FMargin(10))
                    .HAlign(HAlign_Left)
                    .MinDesiredSlotWidth(90.f)
                    .MinDesiredSlotHeight(75.f)
                    .MaxDesiredSlotWidth(90.f)
                    .MaxDesiredSlotHeight(75.f)
                    // Delete Button
                    + SUniformWrapPanel::Slot()
                    [
                        SNew(SObjectPoolActionButton)
                        .ButtonText(LOCTEXT("ClearPoolText", "Clear Pool"))
                        .IconBrush(FAppStyle::GetBrush("Icons.Delete"))
                        .OnClicked(this, &SObjectPoolQuickAction::OnClearAllPoolClicked)
                    ]
                    // Shrink All Pool Button
                    + SUniformWrapPanel::Slot()
                    [
                        SNew(SObjectPoolActionButton)
                        .ButtonText(LOCTEXT("ShrinkPoolText", "Shrink Pool"))
                        .IconBrush(FAppStyle::GetBrush("Icons.Transform"))
                        .OnClicked(this, &SObjectPoolQuickAction::OnShrinkAllPoolClicked)
                    ]
                ]
                    
                // Stats
                + SHorizontalBox::Slot()
                .Padding(FMargin(0, 0, 20, 0))
                .FillWidth(0.3f)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SNew(SVerticalBox)
                    
                    // Next Pool Shrink
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("NextPoolShrinkLabel", "Pool Shrink In"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
                        .ColorAndOpacity(LightTextColor)
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(this, &SObjectPoolQuickAction::GetNextPoolShrinkTime)
                        .Justification(ETextJustify::Center)
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                        .ColorAndOpacity(LightTextColor)
                    ]
                    
                    // Last Accessed
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 5, 0, 0)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("LastAccessedLabel", "Last Accessed"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
                        .ColorAndOpacity(LightTextColor)
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(this, &SObjectPoolQuickAction::GetLastAccessTime)
                        .Justification(ETextJustify::Center)
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
                        .ColorAndOpacity(LightTextColor)
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10)
            [
                SNew(SOverlay)
                // Progress Bar
                + SOverlay::Slot()
                [
                    SNew(SProgressBar)
                    .Percent(this, &SObjectPoolQuickAction::GetTotalPoolSizeRatio)
                    .Style(FLazyGenericDynamicObjectPoolStyle::Get(), "OrangeFillProgressBar")
                ]
                    
                // Stats Text
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Center)
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)
                    // Total Actor
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(STextBlock)
                        .Text(this, &SObjectPoolQuickAction::GetTotalActorText)
                        .ColorAndOpacity(LightTextColor)
                    ]
                        
                    // Max Pool Size
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(STextBlock)
                        .Text(this, &SObjectPoolQuickAction::GetMaxPoolSizeText)
                        .ColorAndOpacity(LightTextColor)
                    ]
                        
                    // Shrink Operations
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(STextBlock)
                        .Text(this, &SObjectPoolQuickAction::GetShrinkOperationsText)
                        .ColorAndOpacity(LightTextColor)
                    ]
                ]
            ]
        ]
    ];
}

FReply SObjectPoolQuickAction::OnClearAllPoolClicked()
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FReply::Unhandled();

    World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->ClearAllPools();
    return FReply::Handled();
}

FReply SObjectPoolQuickAction::OnShrinkAllPoolClicked()
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FReply::Unhandled();

    World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->ShrinkAllPools();
    return FReply::Handled();
}

void SObjectPoolQuickAction::OnActorClassSelected(const UClass* Class)
{
    SelectedClass = TSubclassOf<AActor>(const_cast<UClass*>(Class));
}

UClass* SObjectPoolQuickAction::GetSelectedActorClass()
{
    return SelectedClass;
}

FText SObjectPoolQuickAction::GetNextPoolShrinkTime() const
{
    float Time = 0;
    if (const UWorld* World = GEditor->PlayWorld)
    {
        Time = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetNextAutoShrinkTime();
    }
    
    return FText::FromString(Time != 0 ? FString::FormatAsNumber(Time).ToUpper() : "DISABLED");
}

FText SObjectPoolQuickAction::GetLastAccessTime() const
{
    if (!SelectedClass)
    {
        return FText::FromString("N/A");
    }

    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText::FromString("N/A");

    const int32 AccessCount = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetPoolAccessCount(SelectedClass);
    return FText::Format(LOCTEXT("LastAccessCountText", "{0} times"), AccessCount);
}

FText SObjectPoolQuickAction::GetTotalActorText() const
{
    int32 TotalActors;
    if (!SelectedClass)
    {
        const UWorld* World = GEditor->PlayWorld;
        if(!IsValid(World)) return FText();
        TotalActors = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetTotalActorsInAllPools();
        
        return FText::Format(LOCTEXT("TotalActorOfClassText", "{0} Total Actor"), TotalActors);
    }
    
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    TotalActors = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetAllActorsInPool(SelectedClass).Num();
    return FText::Format(LOCTEXT("TotalActorOfClassText", "{0} Total Actor Of Class {1}"), TotalActors, FText::FromString(SelectedClass->GetName()));
}

FText SObjectPoolQuickAction::GetMaxPoolSizeText() const
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    const int32 PoolSize = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetMaximumPoolSize();
    // Implement logic to get max pool size
    return FText::Format(LOCTEXT("MaxPoolSizeText", "{0} Max Pool Size"), PoolSize);
}

FText SObjectPoolQuickAction::GetShrinkOperationsText() const
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    const int32 ShrinkOperations = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetTotalShrinkOperations();
    // Implement logic to get shrink operations count
    return FText::Format(LOCTEXT("ShrinkOperationsText", "{0} Shrink Operations Completed"), ShrinkOperations);
}

FText SObjectPoolQuickAction::GetActorsOfClassInUse() const
{
    if (!SelectedClass)
    {
        return LOCTEXT("TotalActorOfClassInUseText", "No Class Selected");
    }

    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    const int32 TotalActors = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetInUseActorsInPool(SelectedClass).Num();
    return FText::Format(LOCTEXT("TotalActorOfClassInUseText", "{0} Actor In Use"), TotalActors);
}

FText SObjectPoolQuickAction::GetActorsOfClassInPool() const
{
    if (!SelectedClass)
    {
        return LOCTEXT("TotalActorOfClassAvailableText", "No Class Selected");
    }

    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    const int32 TotalActors = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetAvailableActorsInPool(SelectedClass).Num();
    return FText::Format(LOCTEXT("TotalActorOfClassAvailableText", "{0} Available Actors"), TotalActors);
}

FText SObjectPoolQuickAction::GetPoolGrowthOperation() const
{
    if (!SelectedClass)
    {
        return LOCTEXT("GrowthOperationText", "No Class Selected");
    }

    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return FText();

    const int32 TotalGrowth = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetPoolGrowthOperation(SelectedClass);
    return FText::Format(LOCTEXT("GrowthOperationText", "{0} Growth Operation"), TotalGrowth);
}

void SObjectPoolQuickAction::SetButtonHoverColor()
{
    ButtonColor = FSlateColor(FColor::Blue);
}


void SObjectPoolQuickAction::ResetButtonColor()
{
    ButtonColor = FSlateColor(FColor::Transparent);
}

TOptional<float> SObjectPoolQuickAction::GetTotalPoolSizeRatio() const
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return TOptional<float>(0);

    return World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetTotalActorsInPoolRatio();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE