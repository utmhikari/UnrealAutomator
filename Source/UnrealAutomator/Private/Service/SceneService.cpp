#include "Service/SceneService.h"

#include "Engine.h"
#include "Log.h"

#include "Blueprint/UserWidget.h"


UWorld* FSceneService::GetCurrentWorld()
{
	if (GEngine != nullptr)
	{
		return GEngine->GetWorld();
	}
	UE_LOG(UALog, Warning, TEXT("Cannot find GEngine! Use UserWidget to find current world!"));
	// find by user widget
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* UserWidget = *Itr;
		if (UserWidget == nullptr || !UserWidget->GetIsVisible() || UserWidget->WidgetTree == nullptr) {
			continue;
		}
		auto World = Itr->GetWorld();
		if (World != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("Found current world via UserWidget!"));
			return World;
		}
	}
	return nullptr;
}
