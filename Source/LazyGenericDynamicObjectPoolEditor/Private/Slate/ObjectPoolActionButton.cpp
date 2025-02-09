// // Copyright (C) 2024 Job Omondiale - All Rights Reserved


#include "Slate/ObjectPoolActionButton.h"

#include "FLazyGenericDynamicObjectPoolStyle.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SObjectPoolActionButton::Construct(const FArguments& InArgs)
{
	ButtonColor = FSlateColor(FColor::Transparent);
	ButtonText = InArgs._ButtonText;
	IconBrush = InArgs._IconBrush;
	ContentPadding = InArgs._ContentPadding;
	WidthOverride = InArgs._WidthOverride;
	HeightOverride = InArgs._HeightOverride;
	OnClickedEvent = InArgs._OnClicked;

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(this, &SObjectPoolActionButton::GetWidthOverride)
			.HeightOverride(this, &SObjectPoolActionButton::GetHeightOverride)
			[
				SNew(SButton)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.ButtonStyle(FLazyGenericDynamicObjectPoolStyle::Get(), "WhiteRoundedButton")
				.ButtonColorAndOpacity(this, &SObjectPoolActionButton::GetButtonColor)
				.OnHovered(this, &SObjectPoolActionButton::SetButtonHoverColor)
				.OnUnhovered(this, &SObjectPoolActionButton::ResetButtonColor)
				.OnClicked(this, &SObjectPoolActionButton::OnButtonClicked)
				.ContentPadding(ContentPadding)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(0, 5)
					[
						SNew(SBox)
						.WidthOverride(20)
						.HeightOverride(20)
						[
							SNew(SImage)
							.ColorAndOpacity(this, &SObjectPoolActionButton::GetIconColor)
							.Image(IconBrush)
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.Padding(0, 5)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Center)
						.Text(ButtonText)
						.ColorAndOpacity(this, &SObjectPoolActionButton::GetTextColor)
					]
				]
			]
		];
}

void SObjectPoolActionButton::SetButtonHoverColor()
{
	ButtonColor = FSlateColor(FColor(0,105,217));
	TextColor = FSlateColor(FLinearColor(0.631f, 0.631f, 0.631f));
	IconColor = FSlateColor(FLinearColor(0.631f, 0.631f, 0.631f));
}

void SObjectPoolActionButton::ResetButtonColor()
{
	ButtonColor = FSlateColor(FColor::Transparent);
	TextColor = FSlateColor(FLinearColor(0.431f, 0.431f, 0.431f));
	IconColor = FSlateColor(FLinearColor(0.431f, 0.431f, 0.431f));
}

FReply SObjectPoolActionButton::OnButtonClicked() const
{
	if (OnClickedEvent.IsBound())
	{
		return OnClickedEvent.Execute();
	}
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
