// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "FLazyGenericDynamicObjectPoolStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FLazyGenericDynamicObjectPoolStyle::StyleInstance = nullptr;

void FLazyGenericDynamicObjectPoolStyle::Initialize()
{
	if(StyleInstance.IsValid()) return;
	
	StyleInstance = Create();
	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

void FLazyGenericDynamicObjectPoolStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FLazyGenericDynamicObjectPoolStyle::ReloadTextures()
{
	if (!FSlateApplication::IsInitialized()) return;
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FLazyGenericDynamicObjectPoolStyle::Get()
{
	return *StyleInstance;
}

FName FLazyGenericDynamicObjectPoolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("FLazyGenericDynamicObjectPoolStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FLazyGenericDynamicObjectPoolStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("FLazyGenericDynamicObjectPoolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LazyGenericDynamicObjectPool")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LazyGenericPoolWindow.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	// Create a rounded border brush
	const FSlateBrush* BorderBrush = new FSlateRoundedBoxBrush(FLinearColor::White, 4.0f);

	// Create a button style using the rounded border brush
	const FButtonStyle ButtonStyle = FButtonStyle()
	.SetNormal(*BorderBrush)
	.SetHovered(FSlateRoundedBoxBrush(FLinearColor::White, 4.0f))
	.SetPressed(FSlateRoundedBoxBrush(FLinearColor::White, 4.0f))
	.SetNormalPadding(FMargin(2.0f));

	Style->Set("WhiteRoundedButton", ButtonStyle);

	// Define the progress bar style
	const FProgressBarStyle ProgressBarStyle = FProgressBarStyle()
	.SetBackgroundImage(FSlateColorBrush(FLinearColor(FColor(0, 0, 0, 40))))
	.SetFillImage(FSlateColorBrush(FLinearColor(1.0f, 0.5f, 0.0f)));

	Style->Set("OrangeFillProgressBar", ProgressBarStyle);

	return Style;
}
