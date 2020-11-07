#include "Handler/PlayerHandler.h"
#include "UALog.h"
#include "Util/CommonUtil.h"
#include "Util/WebUtil.h"
#include "Service/PlayerService.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "GameFramework/Pawn.h"
#include "Misc/DefaultValueHelper.h"
#include "GameFramework/InputSettings.h"
#include "Runtime/InputCore/Classes/InputCoreTypes.h"


const float FPlayerHandler::DEFAULT_SETLOC_DROP_OFFSET = 150.0;


TUniquePtr<FHttpServerResponse> FPlayerHandler::GetPlayerInputs(const FHttpServerRequest& Request)
{
	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (Settings == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get project input settings!"));
	}

	// get action mappings
	TArray<TSharedPtr<FJsonValue>> ActionMappings;
	TArray<FInputActionKeyMapping> InputActionMappings(Settings->GetActionMappings());
	for (auto InputAction : InputActionMappings)
	{
		TSharedPtr<FJsonObject> ActionMapping = MakeShareable(new FJsonObject());
		ActionMapping->SetStringField(TEXT("Name"), InputAction.ActionName.ToString());
		ActionMapping->SetStringField(TEXT("Key"), InputAction.Key.GetFName().ToString());
		ActionMapping->SetBoolField(TEXT("bAlt"), InputAction.bAlt);
		ActionMapping->SetBoolField(TEXT("bCtrl"), InputAction.bCtrl);
		ActionMapping->SetBoolField(TEXT("bShift"), InputAction.bShift);
		ActionMapping->SetBoolField(TEXT("bCmd"), InputAction.bCmd);
		ActionMappings.Push(MakeShareable(new FJsonValueObject(ActionMapping)));
	}

	// get axis mappings
	TArray<TSharedPtr<FJsonValue>> AxisMappings;
	TArray<FInputAxisKeyMapping> InputAxisMappings(Settings->GetAxisMappings());
	for (auto InputAxis : InputAxisMappings)
	{
		TSharedPtr<FJsonObject> AxisMapping = MakeShareable(new FJsonObject());
		AxisMapping->SetStringField(TEXT("Name"), InputAxis.AxisName.ToString());
		AxisMapping->SetStringField(TEXT("Key"), InputAxis.Key.GetFName().ToString());
		AxisMapping->SetNumberField(TEXT("Scale"), InputAxis.Scale);
		AxisMappings.Push(MakeShareable(new FJsonValueObject(AxisMapping)));
	}

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetArrayField(TEXT("ActionMappings"), ActionMappings);
	Body->SetArrayField(TEXT("AxisMappings"), AxisMappings);
	return FWebUtil::SuccessResponse(Body, ("Successfully got inputs!"));
}

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
	Location->SetNumberField(TEXT("X"), PlayerLocation.X);
	Location->SetNumberField(TEXT("Y"), PlayerLocation.Y);
	Location->SetNumberField(TEXT("Z"), PlayerLocation.Z);
	// get rotation
	FRotator PlayerRotation = PlayerPawn->GetActorRotation();
	TSharedPtr<FJsonObject> Rotation = MakeShareable(new FJsonObject());
	Rotation->SetNumberField(TEXT("Pitch"), PlayerRotation.Pitch);
	Rotation->SetNumberField(TEXT("Yaw"), PlayerRotation.Yaw);
	Rotation->SetNumberField(TEXT("Eoll"), PlayerRotation.Roll);
	// generate body
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetObjectField(TEXT("Location"), Location);
	Body->SetObjectField(TEXT("Rotation"), Rotation);
	// get name
	Body->SetStringField(TEXT("Name"), PlayerPawn->GetName());
	// get class name
	Body->SetStringField(TEXT("Name"), PlayerPawn->GetClass()->GetName());
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

	// get player pawn
	APawn* PlayerPawn = FPlayerService::GetPlayerPawn();
	if (PlayerPawn == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get valid player pawn instance!"));
	}

	// set new location
	FVector NewLocation;
	NewLocation.X = RequestBody->GetNumberField(TEXT("X"));
	NewLocation.Y = RequestBody->GetNumberField(TEXT("Y"));
	NewLocation.Z = RequestBody->GetNumberField(TEXT("Z"));

	// check if drop
	const FString* IsDropStr = Request.QueryParams.Find(TEXT("IsDrop"));
	bool IsDrop = IsDropStr != nullptr && IsDropStr->ToBool();
	if (IsDrop)
	{
		float DropOffset = 0.0;
		const FString* DropOffsetStr = Request.QueryParams.Find(TEXT("DropOffset"));
		if (DropOffsetStr != nullptr)
		{
			FDefaultValueHelper::ParseFloat(*DropOffsetStr, DropOffset);
		}
		if (DropOffset <= 0.0)
		{
			DropOffset = DEFAULT_SETLOC_DROP_OFFSET;
		}
		UE_LOG(LogUnrealAutomator, Log, TEXT("Set player location with drop offset: %f"), DropOffset);
		NewLocation.Z += DropOffset;
	}

	UE_LOG(LogUnrealAutomator, Log, TEXT("Set player location to (%.2f, %.2f, %.2f)"), NewLocation.X, NewLocation.Y, NewLocation.Z);

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
	NewRotation.Pitch = RequestBody->GetNumberField("Pitch");
	NewRotation.Yaw = RequestBody->GetNumberField("Yaw");
	NewRotation.Roll = RequestBody->GetNumberField("Roll");
	UE_LOG(LogUnrealAutomator, Log, TEXT("Set player new rotation to (pitch: %.2f, yaw: %.2f, roll: %.2f)"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll);
	if (!PlayerPawn->SetActorRotation(NewRotation, ETeleportType::ResetPhysics))
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to set player rotation!"));
	}
	return FWebUtil::SuccessResponse(TEXT("Player rotation set successfully!"));
}
