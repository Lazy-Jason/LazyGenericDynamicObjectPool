// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "Slate/SObjectPoolQuickAction.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "PropertyCustomizationHelpers.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "Widgets/Layout/SUniformGridPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"

void SObjectPoolQuickAction::Construct(const FArguments& InArgs)
{
    constexpr FLinearColor SectionColor  = FLinearColor(0.075f, 0.075f, 0.075f, 1.0f);

    
    // Create a custom style for our progress bar
    FProgressBarStyle CustomStyle = FAppStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar");
    CustomStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0)); // Transparent background
    CustomStyle.FillImage.TintColor = FSlateColor(FLinearColor(0.137f, 0.137f, 0.137f, 1.0f)); // Dark grey fill (35, 35, 35)
    
    ChildSlot
    [
        SNew(SBox)
        .MinDesiredHeight(200)
        [
            SNew(SBorder)
            /*.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))*/
            .ColorAndOpacity(SectionColor)
            .Padding(FMargin(5))
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .Padding(7.0f)
                .AutoHeight()
                .MaxHeight(70)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    [
                        SNew(SClassPropertyEntryBox)
                        .MetaClass(AActor::StaticClass())
                        .SelectedClass(this, reinterpret_cast<TAttribute<const UClass*>::FGetter::TConstMethodPtr<SObjectPoolQuickAction>>(&
                                           SObjectPoolQuickAction::GetSelectedActorClass))
                        .OnSetClass(this, &SObjectPoolQuickAction::OnActorClassSelected)
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SBorder)
                        .ColorAndOpacity(FColor(32, 32, 32))
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .Justification(ETextJustify::Center)
                            .TextStyle(FAppStyle::Get(), "NormalText.Important")
                            .Text(this, &SObjectPoolQuickAction::GetActorsOfClassInUse)
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    [
                        SNew(SBorder)
                        .ColorAndOpacity(FColor(32, 32, 32))
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .Justification(ETextJustify::Center)
                            .TextStyle(FAppStyle::Get(), "NormalText.Important")
                            .Text(this, &SObjectPoolQuickAction::GetActorsOfClassInPool)
                        ]
                    ]
                ]
                
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(10.0f)
                    [
                        SNew(SUniformGridPanel)
                        .SlotPadding(FMargin(3))
                        + SUniformGridPanel::Slot(0, 0)
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Top)
                        [
                            SNew(SButton)
                            .ContentPadding(30)
                            .OnClicked(this, &SObjectPoolQuickAction::OnClearAllPoolClicked)
                            [
                                SNew(SImage)
                                .Image(FAppStyle::GetBrush("Icons.Delete"))
                                .ColorAndOpacity(FColor::White)
                                .DesiredSizeOverride(FVector2d{30, 30})
                            ]
                        ]
                        + SUniformGridPanel::Slot(1, 0)
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Top)
                        [
                            SNew(SButton)
                            .ContentPadding(30)
                            .OnClicked(this, &SObjectPoolQuickAction::OnClearAllPoolClicked)
                            [
                                SNew(SImage)
                                .Image(FAppStyle::GetBrush("Icons.Transform"))
                                .ColorAndOpacity(FColor::White)
                                .DesiredSizeOverride(FVector2d{30, 30})
                            ]
                        ]
                    ]
                    
                    + SHorizontalBox::Slot()
                    [
                        SNew(SBox)
                        .MaxDesiredWidth(50)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            [
                                SNew(SVerticalBox)
                                + SVerticalBox::Slot()
                                [
                                    SNew(STextBlock)
                                    .AutoWrapText(true)
                                    .Justification(ETextJustify::Center)
                                    .TextStyle(FAppStyle::Get(), "NormalText.Important")
                                    .Text(LOCTEXT("NextPoolShrinkTimeText", "Next Pool Shrink"))
                                ]
                                + SVerticalBox::Slot()
                                .HAlign(HAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .AutoWrapText(true)
                                    .Justification(ETextJustify::Center)
                                    .TextStyle(FAppStyle::Get(), "NormalText.Important")
                                    .Text(this, &SObjectPoolQuickAction::GetNextPoolShrinkTime)
                                ]
                            ]
                            
                            + SVerticalBox::Slot()
                            [
                                SNew(SVerticalBox)
                                + SVerticalBox::Slot()
                                [
                                    SNew(STextBlock)
                                    .AutoWrapText(true)
                                    .Justification(ETextJustify::Center)
                                    .TextStyle(FAppStyle::Get(), "NormalText.Important")
                                    .Text(LOCTEXT("LastAccessTimeText", "Last Accessed"))
                                ]
                                + SVerticalBox::Slot()
                                .HAlign(HAlign_Center)
                                [
                                    SNew(STextBlock)
                                    .AutoWrapText(true)
                                    .Justification(ETextJustify::Center)
                                    .TextStyle(FAppStyle::Get(), "SubtitleText")
                                    .Text(this, &SObjectPoolQuickAction::GetLastAccessTime)
                                ]
                            ]
                        ]
                    ]
                ]
                
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 10, 0, 0)
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    [
                        SNew(SBox)
                        .MinDesiredHeight(25)
                        [
                            SNew(SProgressBar)
                            .Percent(this, &SObjectPoolQuickAction::GetTotalPoolSizeRatio)
                            .FillColorAndOpacity(FLinearColor(32, 32, 32))
                        ]
                    ]
                    
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Center)
                    .Padding(5.0f)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .Text(this, &SObjectPoolQuickAction::GetTotalActorText)
                            .Justification(ETextJustify::Center)
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .Text(this, &SObjectPoolQuickAction::GetMaxPoolSizeText)
                            .Justification(ETextJustify::Center)
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(STextBlock)
                            .AutoWrapText(true)
                            .Text(this, &SObjectPoolQuickAction::GetShrinkOperationsText)
                            .Justification(ETextJustify::Center)
                        ]
                    ]
                ]
            ]
        ]
    ];
}

FReply SObjectPoolQuickAction::OnClearAllPoolClicked()
{
    // Implement clear all pool logic
    return FReply::Handled();
}

FReply SObjectPoolQuickAction::OnShrinkAllPoolClicked()
{
    // Implement shrink all pool logic
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
    
    return FText::FromString(Time != 0 ? FString::FormatAsNumber(Time) : "Disabled");
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

TOptional<float> SObjectPoolQuickAction::GetTotalPoolSizeRatio() const
{
    const UWorld* World = GEditor->PlayWorld;
    if(!IsValid(World)) return TOptional<float>(0);

    return World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>()->GetTotalActorsInPoolRatio();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE