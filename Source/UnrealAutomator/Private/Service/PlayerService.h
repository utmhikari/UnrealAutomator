#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Controller.h"

namespace UnrealAutomator
{
	class FPlayerService
	{
	public:
		static APawn* GetPlayerPawn();
	};
}