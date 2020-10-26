#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Model/SceneModel.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"



/**
 * A manager handling the macroscopic aspects
 */
class FSceneService
{
public:
	/**
	 * Get current UWorld instance
	 */
	static UWorld* GetWorld();

	/**
	 * Get specific actor by ID (to json)
	 */
	static TSharedPtr<FJsonObject> GetActorByID(int32 ID);

	/**
	 * Get actors in current level (to json)
	 */
	static TArray<TSharedPtr<FJsonValue>> GetActors(FSceneActorQuery Query);

private:
	/**
	 * Actor to json
	 */
	static TSharedPtr<FJsonObject> ActorToJson(AActor* Actor);
};
