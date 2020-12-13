/**
 * UnrealAutomator AutoTest
 */

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Service/UIService.h"
#include "Service/SceneService.h"
#include "Service/PlayerService.h"
#include "Util/CommonUtil.h"
#include "UALog.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnrealAutomatorWidgetTest, "UnrealAutomator.Widget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnrealAutomatorWidgetTest::RunTest(const FString& Parameters)
{
	UE_LOG(LogUnrealAutomator, Log, TEXT("Start UA Widget Test..."));
	auto WidgetJson = FUIService::GetWidgetTreeJson();
	auto WidgetJsonString = FCommonUtil::JsonStringify(WidgetJson);
	UE_LOG(LogUnrealAutomator, Log, TEXT("Current widget: %s"), *WidgetJsonString);
	return !WidgetJsonString.IsEmpty() && !WidgetJsonString.Equals(TEXT("{}"));
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUnrealAutomatorActorTest, "UnrealAutomator.Actor", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUnrealAutomatorActorTest::RunTest(const FString& Parameters)
{
	UE_LOG(LogUnrealAutomator, Log, TEXT("Start UA Actor Test..."));
	auto CurActors = FSceneService::GetAllActors();
	if (CurActors.Num() == 0)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("No actors right now!"));
		return false;
	}
	/*auto PlayerPawn = FPlayerService::GetPlayerPawn();
	if (PlayerPawn == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("No player pawn right now!"));
		return false;
	}
	FString PlayerPawnClassName = PlayerPawn->GetClass()->GetName();
	UE_LOG(LogUnrealAutomator, Warning, TEXT("Player class name: %s!"), *PlayerPawnClassName);*/

	TSet<FString> ClassNameSet;

	for (auto CurActor : CurActors)
	{
		if (auto ActorJsonObj = CurActor->AsObject())
		{
			FString ActorClassName;
			if (ActorJsonObj->TryGetStringField(TEXT("Class"), ActorClassName))
			{
				if (!ClassNameSet.Contains(ActorClassName))
				{
					if (ActorClassName.Equals(TEXT("PlayerPawn_C")))
					{
						UE_LOG(LogUnrealAutomator, Log, TEXT("Found player pawn with ID: %d, Name: %s"), ActorJsonObj->GetIntegerField(TEXT("ID")), *ActorJsonObj->GetStringField(TEXT("Name")));
						return true;
					}
					ClassNameSet.Add(ActorClassName);
				}
			}
		}
	}
	UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot find player pawn instance by class name! Current: %s"), *FString::Join(ClassNameSet, TEXT(",")));
	return false;
}