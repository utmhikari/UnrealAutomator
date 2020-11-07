#include "PlayerService.h"
#include "UALog.h"
#include "Engine.h"
#include "Util/InspectUtil.h"
#include "Service/SceneService.h"
#include "Components/InputComponent.h"


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
	auto World = FSceneService::GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("No world, no player controller!"));
		return nullptr;
	}

	return World->GetFirstPlayerController();
}


FString FPlayerService::ConvertInputEventToString(int32 Value)
{
	switch (Value)
	{
	case EInputEvent::IE_Pressed:
		return TEXT("IE_Pressed");
	case EInputEvent::IE_Released:
		return TEXT("IE_Released");
	case EInputEvent::IE_Repeat:
		return TEXT("IE_Repeat");
	case EInputEvent::IE_DoubleClick:
		return TEXT("IE_DoubleClick");
	case EInputEvent::IE_Axis:
		return TEXT("IE_Axis");
	case EInputEvent::IE_MAX:
		return TEXT("IE_MAX");
	default:
		return TEXT("");
	}
}

