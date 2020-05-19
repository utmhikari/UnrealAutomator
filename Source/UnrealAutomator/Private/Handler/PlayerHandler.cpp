#include "Handler/PlayerHandler.h"
#include "Util/WebUtil.h"
#include "Service/PlayerService.h"


namespace UnrealAutomator
{
	TUniquePtr<FHttpServerResponse> FPlayerHandler::GetPlayerLocation(const FHttpServerRequest& Request)
	{
		FVector PlayerLocation = FPlayerService::GetPlayerLocation();
		TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
		Body->SetNumberField(TEXT("x"), PlayerLocation.X);
		Body->SetNumberField(TEXT("y"), PlayerLocation.Y);
		Body->SetNumberField(TEXT("z"), PlayerLocation.Z);
		return FWebUtil::SuccessResponse(Body);
	}
}