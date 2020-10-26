#pragma once

#include "CoreMinimal.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "GameFramework/Controller.h"


class FPlayerService
{
public:
	/**
	 * Get player pawn
	 */
	static APawn* GetPlayerPawn();

	/**
	 * Get player controller
	 */
	static APlayerController* GetPlayerController();

private:
	/**
	 * Convert input event enum to string
	 */
	static FString ConvertInputEventToString(int32 Value);
};
