#include "Service/SceneService.h"

#include "Engine.h"
#include "Log.h"

#include "Blueprint/UserWidget.h"

namespace UnrealAutomator
{
	UWorld* FSceneService::GetCurrentWorld()
	{
		if (GEngine == nullptr)
		{
			UE_LOG(UALog, Warning, TEXT("Cannot find GEngine!"));
			return nullptr;
		}
		UWorld* World = GEngine->GetWorld();
		if (World != nullptr)
		{
			return World;
		}
		// find by user widget
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* UserWidget = *Itr;
			if (UserWidget == nullptr || !UserWidget->GetIsVisible() || UserWidget->WidgetTree == nullptr) {
				continue;
			}
			World = Itr->GetWorld();
			if (World != nullptr)
			{
				UE_LOG(UALog, Log, TEXT("Found current world via UserWidget!"));
				return World;
			}
		}
		// try get by first world context?
		auto WorldContexts = GEngine->GetWorldContexts();
		if (WorldContexts.Num() == 0)
		{
			UE_LOG(UALog, Warning, TEXT("No world context!"));
			return nullptr;
		}
		return WorldContexts[0].World();
	}
}