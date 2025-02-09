// // Copyright (C) 2024 Job Omondiale - All Rights Reserved


#include "Slate/ObjectPoolDebugger.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "SlateOptMacros.h"
#include "Slate/ObjectPoolSearchAction.h"
#include "Slate/SObjectPoolQuickAction.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"


void SObjectPoolDebugger::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		[
			SNew(SWidgetSwitcher)
			.WidgetIndex_Lambda([]() { return GEditor->PlayWorld ? 1 : 0; })

			// Widget index 0: Not in PIE
			+ SWidgetSwitcher::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.HeightOverride(50)
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FColor::Red)
					[
						SNew(SBox)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.AutoWrapText(true)
							.Justification(ETextJustify::Center)
							.Text(LOCTEXT("AvailableOnlyInPIETEXT", "AVAILABLE ONLY IN PIE"))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
							.ColorAndOpacity(FColor::White)
						]
					]
				]
			]

			// Widget index 1: In PIE
			+ SWidgetSwitcher::Slot()
			[
				SNew(SSplitter)
				.Orientation(Orient_Vertical)
				+ SSplitter::Slot()
				.Value(0.5f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SAssignNew(ObjectPoolQuickAction, SObjectPoolQuickAction)
					]
				]
				+ SSplitter::Slot()
				.Value(0.5f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SAssignNew(ObjectPoolSearchAction, SObjectPoolSearchAction)
					]
				]
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE