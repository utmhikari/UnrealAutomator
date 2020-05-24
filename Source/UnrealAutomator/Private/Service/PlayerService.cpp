#include "PlayerService.h"
#include "Log.h"
#include "Engine.h"

#include "Service/SceneService.h"


namespace UnrealAutomator
{
	/* ================= Public Methods ==================== */

	APawn* FPlayerService::GetPlayerPawn()
	{
		auto PlayerController = GetPlayerController();
		if (PlayerController == nullptr)
		{
#if WITH_EDITOR
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Failed to get player controller!"));
			}
#endif
			return nullptr;
		}
		auto PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn == nullptr)
		{
			UE_LOG(UALog, Warning, TEXT("Player controller is not binding a pawn!"));
#if WITH_EDITOR
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Player is not a pawn or not under control!"));
			}
#endif
		}
		return PlayerPawn;
	}

	APlayerController* FPlayerService::GetPlayerController()
	{
		TArray<APlayerController*> PlayerControllers;
		GEngine->GetAllLocalPlayerControllers(PlayerControllers);
		if (PlayerControllers.Num() == 0)
		{
			UE_LOG(UALog, Warning, TEXT("No player controller!"));
			return nullptr;
		}
		return PlayerControllers[0];
	}
}
