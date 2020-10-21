#include "PlayerService.h"
#include "Log.h"
#include "Engine.h"
#include "Service/SceneService.h"



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
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Player controller is not binding a pawn!"));
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
	auto World = FSceneService::GetCurrentWorld();
	if (World == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("No world, no player controller!"));
		return nullptr;
	}

	return World->GetFirstPlayerController();
}

