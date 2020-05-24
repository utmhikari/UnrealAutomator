#include "Handler/UIHandler.h"
#include "Service/UIService.h"
#include "Util/WebUtil.h"


namespace UnrealAutomator
{
	TUniquePtr<FHttpServerResponse> FUIHandler::GetWidgetTree(const FHttpServerRequest& Request)
	{
		auto WidgetTree = FUIService::GetWidgetTreeJson();
		return FWebUtil::SuccessResponse(WidgetTree);
	}
}
