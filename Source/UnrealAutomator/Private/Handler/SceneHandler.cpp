#include "Handler/SceneHandler.h"
#include "Service/SceneService.h"
#include "Util/WebUtil.h"
#include "Misc/DefaultValueHelper.h"
#include "Engine.h"
#include "Model/SceneModel.h"
#include "Log.h"


TUniquePtr<FHttpServerResponse> FSceneHandler::GetCurrentLevelInfo(const FHttpServerRequest& Request)
{
	UE_LOG(LogUnrealAutomator, Log, TEXT("Get current level info..."));
	UWorld* World = FSceneService::GetWorld();
	if (World == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Cannot get ID query!"));
	}
	FString MapName = World->GetMapName();
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("MapName"), MapName);
	return FWebUtil::SuccessResponse(Body);
}

TUniquePtr<FHttpServerResponse> FSceneHandler::GetActorInfo(const FHttpServerRequest& Request)
{
	const FString* IDStr = Request.QueryParams.Find(TEXT("ID"));
	if (IDStr == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Cannot get ID query!"));
	}

	int32 ID;
	if (!FDefaultValueHelper::ParseInt(*IDStr, ID))
	{
		return FWebUtil::ErrorResponse(TEXT("ID is not integer!"));
	}

	TSharedPtr<FJsonObject> ActorInfo = FSceneService::GetActorByID(ID);
	if (ActorInfo == nullptr)
	{
		return FWebUtil::ErrorResponse(FString::Printf(TEXT("Cannot get actor with ID %d!"), ID));
	}
	return FWebUtil::SuccessResponse(ActorInfo);
}


TUniquePtr<FHttpServerResponse> FSceneHandler::GetActorsInfo(const FHttpServerRequest& Request)
{
	FSceneActorQuery Query;
	if (!FWebUtil::GetRequestUStructBody(Request, &Query))
	{
		return FWebUtil::ErrorResponse(TEXT("Invalid actor query!"));
	}

	auto ActorsInfo = FSceneService::GetActors(Query);
	if (ActorsInfo.Num() == 0)
	{
		return FWebUtil::ErrorResponse(TEXT("No actors queried!"));
	}
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetArrayField(TEXT("Actors"), ActorsInfo);
	return FWebUtil::SuccessResponse(Body);
}

