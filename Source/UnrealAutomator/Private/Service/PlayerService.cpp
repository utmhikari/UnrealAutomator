#include "PlayerService.h"

#include "Engine.h"

namespace UnrealAutomator
{
	/**
	 * Get location of current player
	 */
	FVector FPlayerService::GetPlayerLocation()
	{
		auto PlayerPawn = GetPlayerPawn();
		if (PlayerPawn == nullptr)
		{
			return FVector(0.0);
		}
		return PlayerPawn->GetActorLocation();
	}


	/* ================= Private Methods ==================== */

	/**
	 * Get player pawn
	 */
	APawn* FPlayerService::GetPlayerPawn()
	{
		if (GEngine == nullptr)
		{
			return nullptr;
		}
		auto World = GEngine->GetWorld();
		if (World == nullptr)
		{
			return nullptr;
		}
		auto FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController == nullptr)
		{
			return nullptr;
		}
		return FirstPlayerController->GetPawn();
	}
}