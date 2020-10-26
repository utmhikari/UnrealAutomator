#include "Handler/CommandHandler.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"

#include "Service/PlayerService.h"


TUniquePtr<FHttpServerResponse> FCommandHandler::ExecuteUECommand(const FHttpServerRequest& Request)
{
	if (GEngine == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("GEngine not found!"));
	}
	TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
	FString Command;
	if (RequestBody == nullptr ||
		!RequestBody->TryGetStringField(TEXT("Cmd"), Command))
	{
		return FWebUtil::ErrorResponse(TEXT("command not found!"));
	}
	auto PlayerController = FPlayerService::GetPlayerController();
	if (PlayerController == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("player controller not found!"));
	}
	PlayerController->ConsoleCommand(Command);
	UE_LOG(LogUnrealAutomator, Log, TEXT("Executed UE command: %s"), *Command);
#if WITH_EDITOR
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Yellow,
			FString::Printf(TEXT("Executed UE command: %s"), *Command));
	}
#endif
	return FWebUtil::SuccessResponse(TEXT("Executed UE command!"));
}

TUniquePtr<FHttpServerResponse> FCommandHandler::ExecuteGMCommand(const FHttpServerRequest& Request)
{
	return FWebUtil::ErrorResponse(TEXT("Not implemented"));
}
