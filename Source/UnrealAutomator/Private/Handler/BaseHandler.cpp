#include "Handler/BaseHandler.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"

namespace UnrealAutomator
{
	
	/**
	 * Health Check
	 */
	bool FBaseHandler::HealthCheck(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
	{
		UE_LOG(UALog, Log, TEXT("Health Check"));
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Health Check Successfully!"));
		}
		OnComplete(FWebUtil::SuccessResponse("Health Check Successfully!"));
		return true;
	}
}