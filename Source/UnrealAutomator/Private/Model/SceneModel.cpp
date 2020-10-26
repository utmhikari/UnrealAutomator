#include "SceneModel.h"
#include "Util/InspectUtil.h"
#include "Log.h"


FString FSceneActorQuery::ToString()
{
	return FString::Printf(TEXT("SceneActorQuery ClassName: %s, Tag: %s"), *ClassName, *Tag);
}

ESceneActorQueryState FSceneActorQuery::GetQueryState()
{
	uint8 Or = static_cast<uint8>(ESceneActorQueryType::Or);
	uint8 And = static_cast<uint8>(ESceneActorQueryType::And);
	if (Type != Or && Type != And)
	{
		Type = And;
	}

	bool bIsClassNameEmpty = ClassName.IsEmpty();
	bool bIsTagEmpty = Tag.IsEmpty();

	if (bIsClassNameEmpty && bIsTagEmpty)
	{
		// ignore And/Or constraints
		return ESceneActorQueryState::All;
	}

	// check if class exists
	if (!bIsClassNameEmpty)
	{
		UClass* Class = FInspectUtil::FindClass(ClassName);
		ActorClass = Class;
		if (ActorClass != nullptr)
		{
			UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to cast actor subclass: %s"), *ClassName);
			if (Type != Or || bIsTagEmpty)
			{
				// invalid class, or no change to query tag
				return ESceneActorQueryState::Invalid;
			}
		}
	}

	if (!bIsClassNameEmpty && !bIsTagEmpty)
	{
		if (Type == Or)
		{
			return ESceneActorQueryState::ClassOrTag;
		}
		else
		{
			return ESceneActorQueryState::ClassAndTag;
		}
	}

	if (bIsClassNameEmpty && !bIsTagEmpty)
	{
		return ESceneActorQueryState::TagOnly;
	}

	if (!bIsClassNameEmpty && bIsTagEmpty)
	{
		return ESceneActorQueryState::ClassOnly;
	}

	// should not reach this line
	return ESceneActorQueryState::Invalid;
}