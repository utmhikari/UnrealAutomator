#include "Service/UIService.h"
#include "Util/CommonUtil.h"
#include "Util/WebUtil.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine.h"
#include "UALog.h"
#include "Service/WidgetService.h"
#include "Service/ViewportService.h"


TSharedPtr<FJsonObject> FUIService::GetWidgetTreeJson()
{
	FViewportService::Reset();

	TSharedPtr<FJsonObject> WidgetRoot = MakeShareable(new FJsonObject());

	UE_LOG(LogUnrealAutomator, Verbose, TEXT("DFS WidgetTree..."));
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* UserWidget = *Itr;
		if (UserWidget == nullptr || UserWidget->WidgetTree == nullptr || !UserWidget->IsVisible())
		{
			UE_LOG(LogUnrealAutomator, Verbose, TEXT("UUserWidget iterator got a null or invisible UUserWidget!"));
			continue;
		}
		TraverseWidget(UserWidget->WidgetTree->RootWidget, WidgetRoot);
	}
	return WidgetRoot;
}


UWidget* FUIService::FindWidget(FUIWidgetQuery Query)
{
	FViewportService::Reset();

	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* UserWidget = *Itr;
		if (UserWidget == nullptr || UserWidget->WidgetTree == nullptr || !UserWidget->IsVisible())
		{
			continue;
		}
		UWidget* Result = FindWidget(UserWidget->WidgetTree->RootWidget, Query, nullptr);
		if (Result != nullptr)
		{
			return Result;
		}
	}
	return nullptr;
}


UWidget* FUIService::FindWidget(UWidget* Root, FUIWidgetQuery Query, UWidget* Parent)
{
	if (Root == nullptr)
	{
		return nullptr;
	}

	EUIWidgetQueryMatchState MatchState = Query.Match(Root);
	UE_LOG(LogUnrealAutomator, Verbose, TEXT("MatchState on %s widget %s (%d) --- %d"),
		*Root->GetClass()->GetName(),
		*Root->GetName(),
		Root->GetUniqueID(),
		static_cast<int8>(MatchState));

	if (MatchState == EUIWidgetQueryMatchState::All)
	{
		return Root;
	}
	if (Parent != nullptr && MatchState == EUIWidgetQueryMatchState::Child)
	{
		return Parent;
	}
	if (MatchState == EUIWidgetQueryMatchState::Parent)
	{
		if (Parent == nullptr)
		{
			Parent = Root;
		}
		else
		{
			// no need to find a second child parent
			MatchState = EUIWidgetQueryMatchState::None;
		}
	}

	if (INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(Root))
	{
		TArray<FName> SlotNames;
		NamedSlotHost->GetSlotNames(SlotNames);
		for (FName SlotName : SlotNames)
		{
			if (UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName))
			{
				UWidget* Result = FindWidget(SlotContent, Query, Parent);
				if (Result != nullptr)
				{
					// should be a MatchAll or current Parent
					return Result;
				}
			}
		}
	}

	if (UPanelWidget* PanelParent = Cast<UPanelWidget>(Root))
	{
		int32 NumChildren = PanelParent->GetChildrenCount();
		for (int32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
		{
			if (UWidget* ChildWidget = PanelParent->GetChildAt(ChildIndex))
			{
				UWidget* Result = FindWidget(ChildWidget, Query, Parent);
				if (Result != nullptr)
				{
					return Result;
				}
			}
		}
	}

	return nullptr;
}


void FUIService::TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent)
{
	if (Widget == nullptr || Parent == nullptr)
	{
		return;
	}

	if (!Parent->HasField(TEXT("Children")))
	{
		Parent->SetObjectField(TEXT("Children"), MakeShareable(new FJsonObject()));
	}

	TSharedPtr<FJsonObject> WidgetJson = FWidgetService::GetWidgetJson(Widget);

	uint32 ParentID;
	if (Parent->TryGetNumberField(TEXT("ID"), ParentID)) {
		WidgetJson->SetNumberField(TEXT("ParentID"), ParentID);
	}

	// traverse children of widget if enabled and visible
	if (Widget->IsVisible() && Widget->GetIsEnabled())
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("Detected UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());

		if (INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(Widget))
		{
			TArray<FName> SlotNames;
			NamedSlotHost->GetSlotNames(SlotNames);
			for (FName SlotName : SlotNames)
			{
				if (UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName))
				{
					TraverseWidget(SlotContent, WidgetJson);
				}
			}
		}

		if (UPanelWidget* PanelParent = Cast<UPanelWidget>(Widget))
		{
			int32 NumChildren = PanelParent->GetChildrenCount();
			for (int32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
			{
				if (UWidget* ChildWidget = PanelParent->GetChildAt(ChildIndex))
				{
					TraverseWidget(ChildWidget, WidgetJson);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("Detected disabled or invisible UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());
	}

	Parent->GetObjectField(TEXT("Children"))
		.ToSharedRef()
		.Get()
		.SetObjectField(FString::FromInt(Widget->GetUniqueID()), WidgetJson);
}
