#pragma once

#include "CoreMinimal.h"
#include "Engine.h"

namespace UnrealAutomator
{
	/**
	 * A manager handling the macroscopic aspects
	 */
	class FSceneService
	{
	public:
		/**
		 * Get current UWorld instance
		 */
		static UWorld* GetCurrentWorld();
	};
}