// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "WebServer.h"

class FUnrealAutomatorModule : public IModuleInterface
{
public:
	FUnrealAutomatorModule() :WebServer(nullptr) {};
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	UnrealAutomator::FWebServer* WebServer;
};
