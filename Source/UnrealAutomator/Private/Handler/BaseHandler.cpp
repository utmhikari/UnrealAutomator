#include "Handler/BaseHandler.h"
#include "Util/WebUtil.h"
#include "Service/BaseService.h"
#include "Engine.h"
#include "Log.h"


TUniquePtr<FHttpServerResponse> FBaseHandler::HealthCheck(const FHttpServerRequest& Request)
{
	UE_LOG(UALog, Log, TEXT("Health Check"));
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Health Check Successfully!"));
	}
	return FWebUtil::SuccessResponse("Health Check Successfully!");
}

TUniquePtr<FHttpServerResponse> FBaseHandler::Screenshot(const FHttpServerRequest& Request)
{
	auto QueryParams = Request.QueryParams;
	FString* ScreenShotName = QueryParams.Find(TEXT("name"));
	bool bSuccess;
	if (ScreenShotName == nullptr)
	{
		bSuccess = FBaseService::Screenshot();
	}
	else
	{
		bSuccess = FBaseService::Screenshot(*ScreenShotName);
	}
	if (!bSuccess)
	{
		return FWebUtil::ErrorResponse("Screenshot failed!");
	}
	return FWebUtil::SuccessResponse(TEXT("Screenshot successfully!"));
}
