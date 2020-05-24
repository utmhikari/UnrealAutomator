#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Controller.h"

namespace UnrealAutomator
{
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
	};
}