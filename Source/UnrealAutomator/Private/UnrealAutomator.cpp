// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UnrealAutomator.h"
#include "WebServer.h"


#define LOCTEXT_NAMESPACE "FUnrealAutomatorModule"

void FUnrealAutomatorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UnrealAutomator::FWebServer::Stop();
	if (!GIsEditor)
	{
		Port = DEFAULT_PORT;
		UnrealAutomator::FWebServer::Start(Port);
	}
}

void FUnrealAutomatorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that supporWt dynamic reloading,
	// we call this function before unloading the module.
	UnrealAutomator::FWebServer::Stop();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealAutomatorModule, UnrealAutomator)