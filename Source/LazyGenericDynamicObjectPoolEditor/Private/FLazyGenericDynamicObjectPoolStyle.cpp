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

	return Style;
}
