#pragma once

#include "CoreMinimal.h"
#include "SceneModel.generated.h"


enum class ESceneActorQueryType : uint8
{
	And = 0,
	Or
};

enum class ESceneActorQueryState : int8
{
	Invalid = 0,
	All,
	ClassAndTag,
	ClassOrTag,
	ClassOnly,
	TagOnly,
};


USTRUCT()
struct FSceneActorQuery
{
	GENERATED_BODY();

public:
	UPROPERTY()
		FString ClassName = TEXT("");

	UPROPERTY()
		FString Tag = TEXT("");

	UPROPERTY()
		uint8 Type = static_cast<uint8>(ESceneActorQueryType::And);

	FSceneActorQuery()
		: ClassName(TEXT(""))
		, Tag(TEXT(""))
		, Type(0)
		, ActorClass(nullptr)
	{};

	FSceneActorQuery(FString InClassName, FString InTag, uint8 InType)
	{
		ClassName = MoveTemp(InClassName);
		Tag = MoveTemp(InTag);
		if (InType != static_cast<uint8>(ESceneActorQueryType::And) && InType != static_cast<uint8>(ESceneActorQueryType::Or))
		{
			InType = static_cast<uint8>(ESceneActorQueryType::And);
		}
		Type = InType;
	}

	FString ToString();

	ESceneActorQueryState GetQueryState();

	FORCEINLINE TSubclassOf<AActor> GetActorClass() { return ActorClass; }

private:
	UPROPERTY()
		TSubclassOf<AActor> ActorClass = nullptr;
};
