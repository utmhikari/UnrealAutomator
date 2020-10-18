#include "Handler/SceneHandler.h"
#include "Service/SceneService.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"


TUniquePtr<FHttpServerResponse> FSceneHandler::GetCurrentMapInfo(const FHttpServerRequest& Request)
{
	UE_LOG(UALog, Log, TEXT("Get current map info..."));
	UWorld* World = FSceneService::GetCurrentWorld();
	if (World == nullptr)
	{
		return FWebUtil::ErrorResponse("Cannot get UWorld instance!");
	}
	FString MapName = World->GetMapName();
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("name"), MapName);
	return FWebUtil::SuccessResponse(Body);
}
