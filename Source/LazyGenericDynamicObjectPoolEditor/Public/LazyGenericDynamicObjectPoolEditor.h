// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLazyGenericDynamicObjectPoolEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** */
	void PluginEditorButtonClicked();

private:
	/**/
	void RegisterMenus();

	/**/
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	/**/
	TSharedPtr<class FUICommandList> PluginCommands;
};
