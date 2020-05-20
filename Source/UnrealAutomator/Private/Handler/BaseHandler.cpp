#include "Handler/BaseHandler.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"

namespace UnrealAutomator
{
	TUniquePtr<FHttpServerResponse> FBaseHandler::HealthCheck(const FHttpServerRequest& Request)
	{
		UE_LOG(UALog, Log, TEXT("Health Check"));
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Health Check Successfully!"));
		}
		return FWebUtil::SuccessResponse("Health Check Successfully!");
	}
}