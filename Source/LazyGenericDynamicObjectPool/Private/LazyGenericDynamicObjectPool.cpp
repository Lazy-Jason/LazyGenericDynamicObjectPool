﻿// Copyright (C) 2024 Job Omondiale - All Rights Reserved

#include "LazyGenericDynamicObjectPool.h"

#define LOCTEXT_NAMESPACE "FLazyGenericDynamicObjectPoolModule"

void FLazyGenericDynamicObjectPoolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLazyGenericDynamicObjectPoolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLazyGenericDynamicObjectPoolModule, LazyGenericDynamicObjectPool)