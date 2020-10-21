#include "Handler/PlayerHandler.h"
#include "Log.h"
#include "Util/WebUtil.h"
#include "Service/PlayerService.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "GameFramework/Pawn.h"


TUniquePtr<FHttpServerResponse> FPlayerHandler::GetPlayerInfo(const FHttpServerRequest& Request)
{
	APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
	if (PlayerPawn == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
	}
	// get location
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	TSharedPtr<FJsonObject> Location = MakeShareable(new FJsonObject());
	Location->SetNumberField(TEXT("x"), PlayerLocation.X);
	Location->SetNumberField(TEXT("y"), PlayerLocation.Y);
	Location->SetNumberField(TEXT("z"), PlayerLocation.Z);
	// get rotation
	FRotator PlayerRotation = PlayerPawn->GetActorRotation();
	TSharedPtr<FJsonObject> Rotation = MakeShareable(new FJsonObject());
	Rotation->SetNumberField(TEXT("pitch"), PlayerRotation.Pitch);
	Rotation->SetNumberField(TEXT("yaw"), PlayerRotation.Yaw);
	Rotation->SetNumberField(TEXT("roll"), PlayerRotation.Roll);
	// generate body
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetObjectField(TEXT("location"), Location);
	Body->SetObjectField(TEXT("rotation"), Rotation);
	return FWebUtil::SuccessResponse(Body);
}

TUniquePtr<FHttpServerResponse> FPlayerHandler::SetPlayerLocation(const FHttpServerRequest& Request)
{
	// get request json and player pawn
	TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
	if (RequestBody == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to parse request body to json!"));
	}

	APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
	if (PlayerPawn == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
	}

	// set new location
	FVector NewLocation;
	NewLocation.X = RequestBody->GetNumberField(TEXT("x"));
	NewLocation.Y = RequestBody->GetNumberField(TEXT("y"));
	NewLocation.Z = RequestBody->GetNumberField(TEXT("z"));
	UE_LOG(LogUnrealAutomator, Log, TEXT("Set player new location to (%.2f, %.2f, %.2f)"), NewLocation.X, NewLocation.Y, NewLocation.Z);

	if (!PlayerPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics))
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to set player location!"));
	}
	return FWebUtil::SuccessResponse(TEXT("Player location set successfully!"));
}

TUniquePtr<FHttpServerResponse> FPlayerHandler::SetPlayerRotation(const FHttpServerRequest& Request)
{
	// get request json and player pawn
	TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
	if (RequestBody == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to parse request body to json!"));
	}
	APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
	if (PlayerPawn == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
	}
	// set new rotation
	FRotator NewRotation;
	NewRotation.Pitch = RequestBody->GetNumberField("pitch");
	NewRotation.Yaw = RequestBody->GetNumberField("yaw");
	NewRotation.Roll = RequestBody->GetNumberField("roll");
	UE_LOG(LogUnrealAutomator, Log, TEXT("Set player new rotation to (pitch: %.2f, yaw: %.2f, roll: %.2f)"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);
	if (!PlayerPawn->SetActorRotation(NewRotation, ETeleportType::ResetPhysics))
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to set player rotation!"));
	}
	return FWebUtil::SuccessResponse(TEXT("Player rotation set successfully!"));
}
