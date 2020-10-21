#include "Service/WidgetService.h"
#include "Log.h"
#include "Engine.h"
#include "Util/InspectUtil.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Service/ViewportService.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/RichTextBlock.h"
#include "UMG/Public/Components/EditableText.h"
#include "UMG/Public/Components/EditableTextBox.h"
#include "UMG/Public/Components/MultiLineEditableTextBox.h"
#include "UMG/Public/Components/MultiLineEditableText.h"


TSharedPtr<FJsonObject> FWidgetService::GetWidgetJson(UWidget* Widget)
{
	TSharedPtr<FJsonObject> WidgetJson = MakeShareable(new FJsonObject());

	// get some basic info
	uint32 WidgetID = Widget->GetUniqueID();
	WidgetJson->SetNumberField(TEXT("ID"), WidgetID);
	WidgetJson->SetStringField(TEXT("Name"), Widget->GetName());
	WidgetJson->SetStringField(TEXT("Class"), Widget->GetClass()->GetName());
	WidgetJson->SetBoolField(TEXT("Visible"), Widget->IsVisible());
	WidgetJson->SetBoolField(TEXT("Enabled"), Widget->GetIsEnabled());

	// try to get text
	const FString* WidgetText = GetWidgetText(Widget);
	if (WidgetText != nullptr)
	{
		WidgetJson->SetStringField(TEXT("Text"), *WidgetText);
	}

	// try to get Z order
	if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		int32 ZOrder = Slot->GetZOrder();
		WidgetJson->SetNumberField(TEXT("ZOrder"), ZOrder);
	}

	// try to get position info
	if (FViewportService::GetIsInited())
	{
		const FGeometry Geometry = Widget->GetCachedGeometry();
		FVector2D WidgetPosition = Geometry.GetAbsolutePosition();
		FVector2D WidgetSize = Geometry.GetAbsoluteSize();

		// GAutomator implementation
		float ViewportWidthScale = FViewportService::GetWidthScale();
		float ViewportHeightScale = FViewportService::GetHeightScale();
		float GAPosX = WidgetPosition.X / ViewportWidthScale;
		float GAPosY = WidgetPosition.Y / ViewportHeightScale;
		float GAWidth = WidgetSize.X / ViewportWidthScale;
		float GAHeight = WidgetSize.Y / ViewportHeightScale;

		// PocoSDK implementation
		/*UObject* WorldContextObject = (UObject*)Widget->GetWorld();
		FVector2D PocoPosOnScreen;
		FVector2D PocoPosInViewport;
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(WorldContextObject);
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(WorldContextObject);
		USlateBlueprintLibrary::AbsoluteToViewport(WorldContextObject, WidgetPosition, PocoPosOnScreen, PocoPosInViewport);*/
		
		// dump info, for debug use
		/*FString Text;
		WidgetJson->TryGetStringField(TEXT("Text"), Text);
		if (Text.Len() > 0)
		{
			if (Text.Len() > 0)
			{
				UE_LOG(LogUnrealAutomator, Warning,
					TEXT("[%s] AbsPosX: %f,AbsPoxY: %f,GAPosX: %f,GAPosY: %f,ScreenPosX: %f,ScreenPosY: %f,VPPosX: %f,VPPosY: %f,ViewportScale: %f,WidthScale: %f,HeightScale: %f,Width: %f,Height: %f,GAWidth: %f,GAHeight: %f"),
					*Text,
					WidgetPosition.X,
					WidgetPosition.Y,
					GAPosX,
					GAPosY,
					PocoPosOnScreen.X,
					PocoPosOnScreen.Y,
					PocoPosInViewport.X,
					PocoPosInViewport.Y,
					ViewportScale,
					ViewportWidthScale,
					ViewportHeightScale,
					WidgetSize.X,
					WidgetSize.Y,
					GAWidth,
					GAHeight);
			}
		}*/
		
		// calculate pos and size
		float PosX = GAPosX;
		float PosY = GAPosY;
		float Width = GAWidth;
		float Height = GAHeight;
		
		// calculate center
		float PosXCenter = FMath::RoundHalfFromZero(PosX + Width * 0.5f);
		float PosYCenter = FMath::RoundHalfFromZero(PosY + Height * 0.5f);

		WidgetJson->SetNumberField(TEXT("PosX"), PosX);
		WidgetJson->SetNumberField(TEXT("PosY"), PosY);
		WidgetJson->SetNumberField(TEXT("Width"), Width);
		WidgetJson->SetNumberField(TEXT("Height"), Height);
		WidgetJson->SetNumberField(TEXT("PosXCenter"), PosXCenter);
		WidgetJson->SetNumberField(TEXT("PosYCenter"), PosYCenter);
	}

#if WITH_EDITOR
	// get some other info when WITH_EDITOR
	WidgetJson->SetStringField(TEXT("Category"), Widget->GetCategoryName());
	WidgetJson->SetStringField(TEXT("Label"), Widget->GetLabelText().ToString());
	WidgetJson->SetStringField(TEXT("DisplayLabel"), Widget->GetDisplayLabel());
	WidgetJson->SetStringField(TEXT("LabelWithMeta"), Widget->GetLabelTextWithMetadata().ToString());
	WidgetJson->SetStringField(TEXT("Meta"), Widget->GetLabelMetadata());
#endif

	return WidgetJson;
}


const FString* FWidgetService::GetWidgetText(UWidget* Widget)
{
	UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
	if (TextBlock != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("GetText: Cast widget %s (%d) as TextBlock..."), *Widget->GetName(), Widget->GetUniqueID());
		// use GetText() to ensure compatible with binding
		return &TextBlock->GetText().ToString();
	}

	URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget);
	if (RichTextBlock != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("GetText: Cast widget %s (%d) as RichTextBlock..."), *Widget->GetName(), Widget->GetUniqueID());
		return &RichTextBlock->GetText().ToString();
	}

	UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(Widget);
	if (EditableTextBox != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("GetText: Cast widget %s (%d) as EditableTextBox..."), *Widget->GetName(), Widget->GetUniqueID());
		return &EditableTextBox->GetText().ToString();
	}

	UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(Widget);
	if (MultiLineEditableTextBox != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("GetText: Casted widget %s (%d) as MultiLineEditableTextBox..."), *Widget->GetName(), Widget->GetUniqueID());
		return &MultiLineEditableTextBox->GetText().ToString();
	}
	return nullptr;
}

bool FWidgetService::SetWidgetText(UWidget* Widget, FString Text)
{
	UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
	if (TextBlock != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Log, TEXT("SetText: Cast widget %d as TextBlock..."), Widget->GetUniqueID());
		TextBlock->SetText(FText::FromString(Text));
		return true;
	}

	URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget);
	if (RichTextBlock != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Log, TEXT("SetText: Cast widget %d as RichTextBlock..."), Widget->GetUniqueID());
		RichTextBlock->SetText(FText::FromString(Text));
		return true;
	}

	UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(Widget);
	if (EditableTextBox != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Log, TEXT("SetText: Cast widget %d as EditableTextBox..."), Widget->GetUniqueID());
		EditableTextBox->SetText(FText::FromString(Text));
		return true;
	}

	UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(Widget);
	if (MultiLineEditableTextBox != nullptr)
	{
		UE_LOG(LogUnrealAutomator, Log, TEXT("SetText: Cast widget %d as MultiLineEditableTextBox..."), Widget->GetUniqueID());
		MultiLineEditableTextBox->SetText(FText::FromString(Text));
		return true;
	}
	return false;
}

bool FWidgetService::InvokeWidgetEvent(UWidget* Widget, FString EventName)
{
	if (EventName.Len() == 0)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to invoke widget event! Empty event name!"));
		return false;
	}

	if (Widget == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Failed to invoke widget event %s! Widget is nullptr!"), *EventName);
		return false;
	}

	FString WidgetName = Widget->GetName();
	int32 WidgetID = Widget->GetUniqueID();
	FString ClassName = Widget->GetClass()->GetName();

	UProperty* Property = FInspectUtil::GetPropertyByName(Widget, EventName);
	if (Property == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Unable to find property %s on %s widget %s (%d)!"), *EventName, *ClassName, *WidgetName, WidgetID);
		return false;
	}

	// event should be a multi-cast delegate without params
	UMulticastDelegateProperty* MultiCastProperty = Cast<UMulticastDelegateProperty>(Property);
	if (MultiCastProperty == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Warning, TEXT("Unable to cast multicast property %s on %s widget %s (%d)!"), *EventName, *ClassName, *WidgetName, WidgetID);
		return false;
	}
	void* PropertyValue = MultiCastProperty->ContainerPtrToValuePtr<void*>(Widget);
	auto MultiCastDelegate = MultiCastProperty->GetMulticastDelegate(PropertyValue);
	MultiCastDelegate->ProcessMulticastDelegate<UObject>(nullptr);

	UE_LOG(LogUnrealAutomator, Warning, TEXT("Successfully invoke event %s of %s widget %s (%d)!"), *EventName, *ClassName, *WidgetName, WidgetID);
	return true;
}