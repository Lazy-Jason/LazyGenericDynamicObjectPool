// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "LazyGenericDynamicObjectPoolEditor.h"

#include "EditorStyleSet.h"
#include "FLazyGenericDynamicObjectPoolStyle.h"
#include "FLazyGenericDynamicObjectPoolCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Filters/SFilterSearchBox.h"
#include "Slate/ObjectPoolSearchAction.h"
#include "Slate/SObjectPoolQuickAction.h"
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"
#include "Widgets/Input/SSearchBox.h"

static const FName LazyGenericDynamicObjectPoolTabName("LazyGenericPoolWindow");

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolEditorModule"

void FLazyGenericDynamicObjectPoolEditorModule::StartupModule()
{
	// initialise the styles // textures
	FLazyGenericDynamicObjectPoolStyle::Initialize();
	FLazyGenericDynamicObjectPoolStyle::ReloadTextures();

	FLazyGenericDynamicObjectPoolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	// when the plugin button is clicked from the toolbar or the window view, perform editor clicked action
	PluginCommands->MapAction(
		FLazyGenericDynamicObjectPoolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLazyGenericDynamicObjectPoolEditorModule::PluginEditorButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLazyGenericDynamicObjectPoolEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LazyGenericDynamicObjectPoolTabName, FOnSpawnTab::CreateRaw(this, &FLazyGenericDynamicObjectPoolEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("LazyGenericPoolWindowTabTitle", "LazyGenericPoolWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FLazyGenericDynamicObjectPoolEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLazyGenericDynamicObjectPoolStyle::Shutdown();

	FLazyGenericDynamicObjectPoolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LazyGenericDynamicObjectPoolTabName);
}

void FLazyGenericDynamicObjectPoolEditorModule::PluginEditorButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LazyGenericDynamicObjectPoolTabName);
}

void FLazyGenericDynamicObjectPoolEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
	Section.AddMenuEntryWithCommandList(FLazyGenericDynamicObjectPoolCommands::Get().OpenPluginWindow, PluginCommands);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& NewSection = ToolbarMenu->FindOrAddSection("PluginTools");
	FToolMenuEntry& Entry = NewSection.AddEntry(FToolMenuEntry::InitToolBarButton(FLazyGenericDynamicObjectPoolCommands::Get().OpenPluginWindow));
	Entry.SetCommandList(PluginCommands);
}

TSharedRef<SDockTab> FLazyGenericDynamicObjectPoolEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SVerticalBox)
            
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
            	SNew(SObjectPoolQuickAction)
            ]
            
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SObjectPoolSearchAction)
            ]
        ];
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLazyGenericDynamicObjectPoolEditorModule, LazyGenericDynamicObjectPoolEditor)