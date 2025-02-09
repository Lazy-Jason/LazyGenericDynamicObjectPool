// // Copyright (C) 2024 Job Omondiale - All Rights Reserved


#include "FunctionLibrary/LazyDynamicObjectPoolLibrary.h"

DEFINE_LOG_CATEGORY(LogLazyDynamicObjectPool);

ULazyDynamicObjectPoolSubsystem* ULazyDynamicObjectPoolLibrary::GetSubsystem(const UObject* ContextObject)
{
	if(!IsValid(ContextObject))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: ContextObject is not valid"));
		return nullptr;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: World is not valid"));
		return nullptr;
	}

	ULazyDynamicObjectPoolSubsystem* Subsystem = World->GetSubsystem<ULazyDynamicObjectPoolSubsystem>();
	if(!IsValid(Subsystem))
	{
		UE_LOG(LogLazyDynamicObjectPool, Warning, TEXT("GetSubsystem: ULazyDynamicObjectPoolSubsystem is not valid"));
	}
	else
	{
		UE_LOG(LogLazyDynamicObjectPool, Log, TEXT("GetSubsystem: Successfully retrieved ULazyDynamicObjectPoolSubsystem"));
	}

	return Subsystem;
}
