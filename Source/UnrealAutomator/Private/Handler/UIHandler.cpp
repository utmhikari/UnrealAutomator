#include "Handler/UIHandler.h"
#include "Service/UIService.h"
#include "Util/WebUtil.h"
#include "Util/CommonUtil.h"
#include "Model/UIModel.h"
#include "Engine.h"
#include "Log.h"


TUniquePtr<FHttpServerResponse> FUIHandler::GetWidgetTree(const FHttpServerRequest& Request)
{
	UE_LOG(UALog, Log, TEXT("Get current UI widget tree..."));
	auto WidgetTree = FUIService::GetWidgetTreeJson();
	return FWebUtil::SuccessResponse(WidgetTree);
}

TUniquePtr<FHttpServerResponse> FUIHandler::GetWidget(const FHttpServerRequest& Request)
{
	FUIWidgetQuery UIWidgetQuery = FUIWidgetQuery();
	if (!FWebUtil::GetRequestUStructBody(Request, &UIWidgetQuery))
	{
		return FWebUtil::ErrorResponse(TEXT("Invalid widget query!"));
	}

	// print on screen to view the query
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, FString::Printf(TEXT("Query widget with id: %d, name: %s, text: %s"),
			UIWidgetQuery.ID,
			*UIWidgetQuery.Name,
			*UIWidgetQuery.Text));
	}

	return FWebUtil::SuccessResponse(TEXT("Get widget successfully!"));
}


TUniquePtr<FHttpServerResponse> FUIHandler::CallWidgetMethod(const FHttpServerRequest& Request)
{
	// TODO: implementation
	TSharedPtr<FJsonObject> Body = FWebUtil::GetRequestJsonBody(Request);
	return FWebUtil::SuccessResponse(TEXT("Call widget method successfully!"));
}	



