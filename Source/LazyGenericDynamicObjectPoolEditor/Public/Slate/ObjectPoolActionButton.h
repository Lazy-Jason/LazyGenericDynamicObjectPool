// // Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"

/**
 * 
 */
class LAZYGENERICDYNAMICOBJECTPOOLEDITOR_API SObjectPoolActionButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SObjectPoolActionButton)
		: _ButtonText(LOCTEXT("DefaultButtonText", "Action"))
		, _IconBrush(FAppStyle::GetBrush("Icons.Delete"))
		, _ContentPadding(FMargin(0))
		, _WidthOverride(35.0f)
		, _HeightOverride(35.0f)
	{}
	SLATE_ATTRIBUTE(FText, ButtonText)
	SLATE_ATTRIBUTE(const FSlateBrush*, IconBrush)
	SLATE_ATTRIBUTE(FMargin, ContentPadding)
	SLATE_ATTRIBUTE(float, WidthOverride)
	SLATE_ATTRIBUTE(float, HeightOverride)
	SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
private:
	FSlateColor ButtonColor;
	FSlateColor TextColor = FSlateColor(FLinearColor(0.431f, 0.431f, 0.431f));
	FSlateColor IconColor = FSlateColor(FLinearColor(0.431f, 0.431f, 0.431f));
	TAttribute<FText> ButtonText;
	TAttribute<const FSlateBrush*> IconBrush;
	TAttribute<FMargin> ContentPadding;
	TAttribute<float> WidthOverride;
	TAttribute<float> HeightOverride;
	FOnClicked OnClickedEvent;

	FSlateColor GetButtonColor() const { return ButtonColor; }
	FSlateColor GetTextColor() const { return TextColor; }
	FSlateColor GetIconColor() const { return IconColor; }
	void SetButtonHoverColor();
	void ResetButtonColor();
	FReply OnButtonClicked() const;
	
	FOptionalSize GetWidthOverride() const { return WidthOverride.Get(); }
	FOptionalSize GetHeightOverride() const { return HeightOverride.Get(); }
};
#undef LOCTEXT_NAMESPACE