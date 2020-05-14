#include "Server.h"
#include "Log.h"

namespace UnrealAutomator
{
	FServer::~FServer() {}

	void FServer::Start()
	{
		UE_LOG(UALog, Log, TEXT("Initializing UnrealAutomator Server..."));
	}
};
