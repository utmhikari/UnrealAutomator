#include "Service/UIService.h"
#include "Util/CommonUtil.h"
#include "Util/WebUtil.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine.h"
#include "Log.h"
#include "Service/WidgetService.h"
#include "Service/ViewportService.h"


TSharedPtr<FJsonObject> FUIService::GetWidgetTreeJson()
{
	// reset viewport
	FViewportService::Reset();

	TSharedPtr<FJsonObject> WidgetRoot = MakeShareable(new FJsonObject());

	UE_LOG(UALog, Verbose, TEXT("DFS WidgetTree..."));
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* UserWidget = *Itr;
		if (UserWidget == nullptr || UserWidget->WidgetTree == nullptr || !UserWidget->IsVisible())
		{
			UE_LOG(UALog, Verbose, TEXT("UUserWidget iterator got a null or invisible UUserWidget!"));
			continue;
		}
		TraverseWidget(UserWidget->WidgetTree->RootWidget, WidgetRoot);
	}
	return WidgetRoot;
}


UWidget* FUIService::FindWidget(FUIWidgetQuery Query)
{
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* UserWidget = *Itr;
		if (UserWidget == nullptr || UserWidget->WidgetTree == nullptr || !UserWidget->IsVisible())
		{
			continue;
		}
		UWidget* Result = FindWidget(UserWidget->WidgetTree->RootWidget, Query);
		if (Result != nullptr)
		{
			return Result;
		}
	}
	return nullptr;
}


UWidget* FUIService::FindWidget(UWidget* Root, FUIWidgetQuery Query)
{
	if (Root == nullptr)
	{
		return nullptr;
	}

	if (Query.IsMatch(Root))
	{
		return Root;
	}

	if (INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(Root))
	{
		TArray<FName> SlotNames;
		NamedSlotHost->GetSlotNames(SlotNames);
		for (FName SlotName : SlotNames)
		{
			if (UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName))
			{
				UWidget* Result = FindWidget(SlotContent, Query);
				if (Result != nullptr)
				{
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
				UWidget* Result = FindWidget(ChildWidget, Query);
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

	if (!Parent->HasField(TEXT("children")))
	{
		Parent->SetObjectField(TEXT("children"), MakeShareable(new FJsonObject()));
	}

	TSharedPtr<FJsonObject> WidgetJson = FWidgetService::GetWidgetJson(Widget);

	// traverse children of widget if enabled and visible
	if (Widget->IsVisible() && Widget->GetIsEnabled())
	{
		UE_LOG(UALog, Verbose, TEXT("Detected UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());

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
		UE_LOG(UALog, Verbose, TEXT("Detected disabled or invisible UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());
	}

	Parent->GetObjectField(TEXT("children"))
		.ToSharedRef()
		.Get()
		.SetObjectField(FString::FromInt(Widget->GetUniqueID()), WidgetJson);
}
