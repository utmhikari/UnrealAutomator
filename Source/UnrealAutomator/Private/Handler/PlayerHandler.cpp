#include "Handler/PlayerHandler.h"
#include "Util/WebUtil.h"
#include "Service/PlayerService.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "GameFramework/Pawn.h"


namespace UnrealAutomator
{
	/**
	 * get player location
	 */
	TUniquePtr<FHttpServerResponse> FPlayerHandler::GetPlayerLocation(const FHttpServerRequest& Request)
	{
		APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
		if (PlayerPawn == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
		}
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
		Body->SetNumberField(TEXT("x"), PlayerLocation.X);
		Body->SetNumberField(TEXT("y"), PlayerLocation.Y);
		Body->SetNumberField(TEXT("z"), PlayerLocation.Z);
		return FWebUtil::SuccessResponse(Body);
	}

	/**
	 * set player location
	 */
	TUniquePtr<FHttpServerResponse> FPlayerHandler::SetPlayerLocation(const FHttpServerRequest& Request)
	{
		TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestBody(Request);
		if (RequestBody == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to parse request body as json!"));
		}
		APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
		if (PlayerPawn == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
		}
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
		// TODO: set location
		return FWebUtil::SuccessResponse(Body);
	}

	/**
	 * get player rotation
	 */
	TUniquePtr<FHttpServerResponse> FPlayerHandler::GetPlayerRotation(const FHttpServerRequest& Request)
	{
		APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
		if (PlayerPawn == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
		}
		FRotator PlayerRotation = PlayerPawn->GetActorRotation();
		TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
		Body->SetNumberField(TEXT("pitch"), PlayerRotation.Pitch);
		Body->SetNumberField(TEXT("yaw"), PlayerRotation.Yaw);
		Body->SetNumberField(TEXT("roll"), PlayerRotation.Roll);
		return FWebUtil::SuccessResponse(Body);
	}

	/**
	 * set player rotation
	 */
	TUniquePtr<FHttpServerResponse> FPlayerHandler::SetPlayerRotation(const FHttpServerRequest& Request)
	{
		TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestBody(Request);
		if (RequestBody == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to parse request body as json!"));
		}
		APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
		if (PlayerPawn == nullptr)
		{
			return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
		}
		FRotator PlayerRotation = PlayerPawn->GetActorRotation();
		TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
		// TODO: set rotation
		return FWebUtil::SuccessResponse(Body);
	}
}