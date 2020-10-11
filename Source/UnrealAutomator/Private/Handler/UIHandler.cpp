#include "Handler/UIHandler.h"
#include "Service/UIService.h"
#include "Util/WebUtil.h"
#include "Util/CommonUtil.h"
#include "Log.h"

namespace UnrealAutomator
{
	TUniquePtr<FHttpServerResponse> FUIHandler::GetWidgetTree(const FHttpServerRequest& Request)
	{
		UE_LOG(UALog, Log, TEXT("Get current UI widget tree..."));
		auto WidgetTree = FUIService::GetWidgetTreeJson();
		return FWebUtil::SuccessResponse(WidgetTree);
	}

	TUniquePtr<FHttpServerResponse> FUIHandler::Screenshot(const FHttpServerRequest& Request)
	{
		auto QueryParams = Request.QueryParams;
		FString* ScreenShotName = QueryParams.Find(TEXT("name"));
		bool Success;
		if (ScreenShotName == nullptr)
		{
			Success = FUIService::Screenshot();
		}
		else
		{
			Success = FUIService::Screenshot(*ScreenShotName);
		}
		if (!Success)
		{
			return FWebUtil::ErrorResponse("Screenshot failed!");
		}
		return FWebUtil::SuccessResponse(TEXT("Screenshot successfully!"));
	}
}
