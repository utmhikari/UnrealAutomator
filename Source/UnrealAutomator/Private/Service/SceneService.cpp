#include "Service/SceneService.h"
#include "Engine.h"
#include "UALog.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


UWorld* FSceneService::GetWorld()
{
	UWorld* World = nullptr;
	if (GEngine != nullptr)
	{
		World = GEngine->GetWorld();
		if (World == nullptr)
		{
			auto WorldContexts = GEngine->GetWorldContexts();
			if (WorldContexts.Num() > 0)
			{
				World = WorldContexts[0].World();
			}
		}
	}
	if (World == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot find current world via GEngine! Use UserWidget to find current world..."));
		// find by user widget
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* UserWidget = *Itr;
			if (UserWidget == nullptr || !UserWidget->GetIsVisible() || UserWidget->WidgetTree == nullptr) {
				continue;
			}
			auto WidgetWorld = Itr->GetWorld();
			if (WidgetWorld != nullptr)
			{
				UE_LOG(LogUnrealAutomator, Log, TEXT("Found current world via UserWidget!"));
				World = WidgetWorld;
				break;
			}
		}
		if (World == nullptr)
		{
			UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to GetWorld!"));
		}
	}
	return World;
}

TSharedPtr<FJsonObject> FSceneService::GetActorByID(int32 ID)
{
	UWorld* World = FSceneService::GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor != nullptr)
		{
			if (ID == Actor->GetUniqueID())
			{
				return ActorToJson(Actor);
			}
		}
	}

	UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot find actor with unique ID %d"), ID);
	return nullptr;
}

TArray<TSharedPtr<FJsonValue>> FSceneService::GetActors(FSceneActorQuery Query)
{
	TArray<TSharedPtr<FJsonValue>> ActorsJson;
	ESceneActorQueryState QueryState = Query.GetQueryState();

	if (QueryState == ESceneActorQueryState::Invalid)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to get actors, query state invalid!"));
		return ActorsJson;
	}

	// get current world
	UWorld* World = FSceneService::GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to get actors, cannot get world!"));
		return ActorsJson;
	}

	if (QueryState == ESceneActorQueryState::All)
	{
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			if (Actor != nullptr)
			{
				ActorsJson.Add(MakeShareable(new FJsonValueObject(ActorToJson(Actor))));
			}
		}
	}
	else
	{
		TArray<AActor*> Actors;

		if (QueryState == ESceneActorQueryState::ClassOrTag)
		{
			// TODO: need optimization
			UGameplayStatics::GetAllActorsOfClass(World, Query.GetActorClass(), Actors);
			TSet<uint32> IDSet;
			for (AActor* ClassActor : Actors)
			{
				if (ClassActor != nullptr)
				{
					IDSet.Add(ClassActor->GetUniqueID());
				}
			}
			TArray<AActor*> TagActors;
			UGameplayStatics::GetAllActorsWithTag(World, FName(*Query.Tag), TagActors);
			for (AActor* TagActor : TagActors)
			{
				if (TagActor != nullptr && !IDSet.Contains(TagActor->GetUniqueID()))
				{
					Actors.Add(TagActor);
				}
			}
		}
		else
		{
			if (QueryState == ESceneActorQueryState::TagOnly)
			{
				UGameplayStatics::GetAllActorsWithTag(World, FName(*Query.Tag), Actors);
			}
			else if (QueryState == ESceneActorQueryState::ClassAndTag)
			{
				UGameplayStatics::GetAllActorsOfClassWithTag(World, Query.GetActorClass(), FName(*Query.Tag), Actors);
			}
			else if (QueryState == ESceneActorQueryState::ClassOnly)
			{
				UGameplayStatics::GetAllActorsOfClass(World, Query.GetActorClass(), Actors);
			}

		}
		for (AActor* Actor : Actors)
		{
			ActorsJson.Add(MakeShareable(new FJsonValueObject(ActorToJson(Actor))));
		}
	}

	UE_LOG(LogUnrealAutomator, Log, TEXT("Queried of actors: %d"), ActorsJson.Num());
	return ActorsJson;
}

TArray<TSharedPtr<FJsonValue>> FSceneService::GetAllActors()
{
	return GetActors(FSceneActorQuery());
}

TSharedPtr<FJsonObject> FSceneService::ActorToJson(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	// Base info
	TSharedPtr<FJsonObject> ActorJson = MakeShareable(new FJsonObject());
	ActorJson->SetNumberField(TEXT("ID"), Actor->GetUniqueID());
	ActorJson->SetStringField(TEXT("Name"), Actor->GetHumanReadableName());
	ActorJson->SetStringField(TEXT("Class"), Actor->GetClass()->GetName());

	// Location
	auto Location = Actor->GetActorLocation();
	TSharedPtr<FJsonObject> LocationJson = MakeShareable(new FJsonObject());
	LocationJson->SetNumberField(TEXT("X"), Location.X);
	LocationJson->SetNumberField(TEXT("Y"), Location.Y);
	LocationJson->SetNumberField(TEXT("Z"), Location.Z);
	ActorJson->SetObjectField(TEXT("Location"), LocationJson);

	// Rotation
	auto Rotation = Actor->GetActorRotation();
	TSharedPtr<FJsonObject> RotationJson = MakeShareable(new FJsonObject());
	RotationJson->SetNumberField(TEXT("Pitch"), Rotation.Pitch);
	RotationJson->SetNumberField(TEXT("Yaw"), Rotation.Yaw);
	RotationJson->SetNumberField(TEXT("Roll"), Rotation.Roll);
	ActorJson->SetObjectField(TEXT("Rotation"), RotationJson);

	// Scale
	auto Scale = Actor->GetActorScale();
	TSharedPtr<FJsonObject> ScaleJson = MakeShareable(new FJsonObject());
	ScaleJson->SetNumberField(TEXT("X"), Scale.X);
	ScaleJson->SetNumberField(TEXT("Y"), Scale.Y);
	ScaleJson->SetNumberField(TEXT("Z"), Scale.Z);
	ActorJson->SetObjectField(TEXT("Scale"), ScaleJson);

	// Components
	auto Components = Actor->GetComponents();
	TArray<TSharedPtr<FJsonValue>> ComponentsJson;
	for (UActorComponent* Component : Components)
	{
		TSharedPtr<FJsonObject> ComponentJson = MakeShareable(new FJsonObject());
		ComponentJson->SetStringField(TEXT("Name"), Component->GetReadableName());
		TArray<TSharedPtr<FJsonValue>> ComponentTagsJson;
		for (FName Tag : Component->ComponentTags)
		{
			ComponentTagsJson.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
		}
		ComponentJson->SetArrayField(TEXT("Tags"), ComponentTagsJson);
		ComponentsJson.Add(MakeShareable(new FJsonValueObject(ComponentJson)));
	}
	ActorJson->SetArrayField(TEXT("Components"), ComponentsJson);

	return ActorJson;
}

