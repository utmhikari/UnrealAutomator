#include "Handler/UIHandler.h"
#include "Service/UIService.h"
#include "Service/WidgetService.h"
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

	UWidget* Widget = FUIService::FindWidget(UIWidgetQuery);
	if (Widget == nullptr)
	{
		UE_LOG(UALog, Warning, TEXT("Failed to get widget json! Cannot find widget!"));
		return FWebUtil::ErrorResponse(TEXT("Cannot find widget!"));
	}

	TSharedPtr<FJsonObject> WidgetJson = FWidgetService::GetWidgetJson(Widget);
	
	if (WidgetJson == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Cannot extract widget json!"));
	}

	return FWebUtil::SuccessResponse(WidgetJson);
}


TUniquePtr<FHttpServerResponse> FUIHandler::InvokeWidgetEvent(const FHttpServerRequest& Request)
{
	const FString* EventName = Request.QueryParams.Find(TEXT("name"));
	if (EventName == nullptr)
	{
		FWebUtil::ErrorResponse(TEXT("Cannot get event to invoke!"));
	}

	FUIWidgetQuery UIWidgetQuery = FUIWidgetQuery();
	if (!FWebUtil::GetRequestUStructBody(Request, &UIWidgetQuery))
	{
		return FWebUtil::ErrorResponse(TEXT("Invalid widget query!"));
	}

	UWidget* Widget = FUIService::FindWidget(UIWidgetQuery);
	if (Widget == nullptr)
	{
		UE_LOG(UALog, Warning, TEXT("Failed to invoke widget event! Cannot find widget!"));
		return FWebUtil::ErrorResponse(TEXT("Cannot find widget!"));
	}

	bool bSuccess = FWidgetService::InvokeWidgetEvent(Widget, *EventName);
	if (!bSuccess)
	{
		return FWebUtil::ErrorResponse(FString::Printf(TEXT("Failed to invoke event %s of widget %s!"), **EventName, *Widget->GetName()));
	}

	return FWebUtil::SuccessResponse(TEXT("Invoke widget event successfully!"));
}	

TUniquePtr<FHttpServerResponse> FUIHandler::SetWidgetText(const FHttpServerRequest& Request)
{
	const FString* Text = Request.QueryParams.Find(TEXT("text"));
	if (Text == nullptr)
	{
		FWebUtil::ErrorResponse(TEXT("Cannot get text to set!"));
	}

	FUIWidgetQuery UIWidgetQuery = FUIWidgetQuery();
	if (!FWebUtil::GetRequestUStructBody(Request, &UIWidgetQuery))
	{
		return FWebUtil::ErrorResponse(TEXT("Invalid widget query!"));
	}

	UWidget* Widget = FUIService::FindWidget(UIWidgetQuery);
	if (Widget == nullptr)
	{
		UE_LOG(UALog, Warning, TEXT("Failed to set widget text! Cannot find widget!"));
		return FWebUtil::ErrorResponse(TEXT("Cannot find widget!"));
	}

	bool bSuccess = FWidgetService::SetWidgetText(Widget, *Text);
	if (!bSuccess)
	{
		return FWebUtil::ErrorResponse(
			FString::Printf(TEXT("Set text for widget %s (%s) failed!"),
				*Widget->GetName(),
				*Widget->GetClass()->GetName()));
	}

	return FWebUtil::SuccessResponse(TEXT("Set widget text successfully!"));
}

