#include "Handler/UIHandler.h"
#include "Log.h"
#include "Util/WebUtil.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"


namespace UnrealAutomator
{
	TUniquePtr<FHttpServerResponse> FUIHandler::GetWidgetTree(const FHttpServerRequest& Request)
	{
		auto WidgetTree = GetWidgetTreeJson();
		return FWebUtil::SuccessResponse(WidgetTree);
	}

	TSharedPtr<FJsonObject> FUIHandler::GetWidgetTreeJson()
	{
		TSharedPtr<FJsonObject> WidgetRoot = MakeShareable(new FJsonObject());
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* UserWidget = *Itr;
			if (UserWidget == nullptr || UserWidget->WidgetTree == nullptr || !UserWidget->IsVisible())
			{
				UE_LOG(UALog, Warning, TEXT("UUserWidget iterator got a null or invisible UUserWidget!"))
				continue;
			}
			TraverseWidget(UserWidget->WidgetTree->RootWidget, WidgetRoot);
		}
		return WidgetRoot;
	}

	void FUIHandler::TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent)
	{
		if (Widget == nullptr || Parent == nullptr)
		{
			return;
		}
		if (!Parent->HasField(TEXT("children")))
		{
			Parent->SetObjectField(TEXT("children"), MakeShareable(new FJsonObject()));
		}
		uint32 WidgetID = Widget->GetUniqueID();
		TSharedPtr<FJsonObject> WidgetJson = MakeShareable(new FJsonObject());

		// fill info
		WidgetJson->SetNumberField(TEXT("id"), WidgetID);
		WidgetJson->SetStringField(TEXT("name"), Widget->GetName());
		WidgetJson->SetStringField(TEXT("class"), Widget->GetClass()->GetName());
		WidgetJson->SetStringField(TEXT("category"), Widget->GetCategoryName());
		WidgetJson->SetStringField(TEXT("label"), Widget->GetLabelText().ToString());
		WidgetJson->SetStringField(TEXT("displayLabel"), Widget->GetDisplayLabel());
		WidgetJson->SetStringField(TEXT("labelWithMeta"), Widget->GetLabelTextWithMetadata().ToString());
		WidgetJson->SetStringField(TEXT("meta"), Widget->GetLabelMetadata());
		WidgetJson->SetBoolField(TEXT("visible"), Widget->IsVisible());
		WidgetJson->SetBoolField(TEXT("enabled"), Widget->GetIsEnabled());

		// traverse children of widget if enabled and visible
		if (Widget->IsVisible() && Widget->GetIsEnabled())
		{
			UE_LOG(UALog, Log, TEXT("Detected UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());
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
			UE_LOG(UALog, Warning, TEXT("Detected disabled or invisible UWidget: %s (%s)"), *Widget->GetName(), *Widget->GetClass()->GetName());
		}

		Parent->GetObjectField(TEXT("children"))
			.ToSharedRef()
			.Get()
			.SetObjectField(FString::FromInt(WidgetID), WidgetJson);
	}
}
