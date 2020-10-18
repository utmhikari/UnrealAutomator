#pragma once

#include "CoreMinimal.h"
#include "Engine.h"


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
