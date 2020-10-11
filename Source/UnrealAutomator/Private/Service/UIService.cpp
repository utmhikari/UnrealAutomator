#include "Service/UIService.h"
#include "Util/CommonUtil.h"
#include "Util/WebUtil.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine.h"
#include "Log.h"
#include "Engine/UserInterfaceSettings.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/RichTextBlock.h"
#include "UMG/Public/Components/EditableText.h"
#include "UMG/Public/Components/EditableTextBox.h"
#include "UMG/Public/Components/MultiLineEditableTextBox.h"
#include "UMG/Public/Components/MultiLineEditableText.h"

namespace UnrealAutomator
{
	bool FUIService::Screenshot(FString Name, bool bEnableSuffix)
	{
		FString Filename;
		if (bEnableSuffix)
		{
			Filename = FString::Printf(TEXT("%s_%s"), *Name, *FCommonUtil::GetCurrentTime());
		}
		else
		{
			Filename = Name;
		}

		UE_LOG(UALog, Log, TEXT("Screenshot to file %s"), *Filename);

		FString Filepath = FString::Printf(TEXT("%sScreenshots/%s/%s"), *FPaths::ProjectSavedDir(), *FString(FPlatformProperties::IniPlatformName()), *Filename);

		// enable high resolution in default
		// GIsHighResScreenshot = true;
		
		FScreenshotRequest::RequestScreenshot(Filepath, true, false);

		return true;
	}

	TSharedPtr<FJsonObject> FUIService::GetWidgetTreeJson()
	{
		// try init viewport scale first
		InitViewport();

		TSharedPtr<FJsonObject> WidgetRoot = MakeShareable(new FJsonObject());
		UE_LOG(UALog, Log, TEXT("DFS WidgetTree..."));
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
		TSharedPtr<FJsonObject> WidgetJson = GetWidgetJson(Widget);
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
			.SetObjectField(FString::FromInt(Widget->GetUniqueID()), WidgetJson);
	}


	TSharedPtr<FJsonObject> FUIService::GetWidgetJson(UWidget* Widget)
	{
		TSharedPtr<FJsonObject> WidgetJson = MakeShareable(new FJsonObject());

		// get some basic info
		uint32 WidgetID = Widget->GetUniqueID();
		WidgetJson->SetNumberField(TEXT("id"), WidgetID);
		WidgetJson->SetStringField(TEXT("name"), Widget->GetName());
		WidgetJson->SetStringField(TEXT("class"), Widget->GetClass()->GetName());
		WidgetJson->SetBoolField(TEXT("visible"), Widget->IsVisible());
		WidgetJson->SetBoolField(TEXT("enabled"), Widget->GetIsEnabled());

		// try get position info
		if (IsViewportInited)
		{
			const FGeometry Geometry = Widget->GetCachedGeometry();
			FVector2D WidgetPosition = Geometry.GetAbsolutePosition();
			FVector2D WidgetSize = Geometry.GetAbsoluteSize();
			WidgetJson->SetNumberField(TEXT("posX"), WidgetPosition.X / WidthScale);
			WidgetJson->SetNumberField(TEXT("posY"), WidgetPosition.Y / HeightScale);
			WidgetJson->SetNumberField(TEXT("width"), WidgetSize.X / WidthScale);
			WidgetJson->SetNumberField(TEXT("height"), WidgetSize.Y / HeightScale);
		}
		
		// try get parent
		UWidget* Parent = Widget->GetParent();
		if (Parent != nullptr)
		{
			WidgetJson->SetNumberField(TEXT("parentID"), Parent->GetUniqueID());
		}

		// try get text
		const FString* WidgetText = GetWidgetText(Widget);
		if (WidgetText != nullptr)
		{
			WidgetJson->SetStringField(TEXT("text"), *WidgetText);
		}
		
#if WITH_EDITOR
		// get some other info when WITH_EDITOR
		WidgetJson->SetStringField(TEXT("category"), Widget->GetCategoryName());
		WidgetJson->SetStringField(TEXT("label"), Widget->GetLabelText().ToString());
		WidgetJson->SetStringField(TEXT("displayLabel"), Widget->GetDisplayLabel());
		WidgetJson->SetStringField(TEXT("labelWithMeta"), Widget->GetLabelTextWithMetadata().ToString());
		WidgetJson->SetStringField(TEXT("meta"), Widget->GetLabelMetadata());
#endif

		return WidgetJson;
	}


	const FString* FUIService::GetWidgetText(UWidget* Widget)
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
		if (TextBlock != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("GetText: Cast widget %d as TextBlock..."), Widget->GetUniqueID());
			// use GetText() to ensure compatible with binding
			return &TextBlock->GetText().ToString();
		}

		URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget);
		if (RichTextBlock != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("GetText: Cast widget %d as RichTextBlock..."), Widget->GetUniqueID());
			return &RichTextBlock->GetText().ToString();
		}

		UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(Widget);
		if (EditableTextBox != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("GetText: Cast widget %d as EditableTextBox..."), Widget->GetUniqueID());
			return &EditableTextBox->GetText().ToString();
		}

		UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(Widget);
		if (MultiLineEditableTextBox != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("GetText: Cast widget %d as MultiLineEditableTextBox..."), Widget->GetUniqueID());
			return &MultiLineEditableTextBox->GetText().ToString();
		}
		return nullptr;
	}

	bool FUIService::SetWidgetText(UWidget* Widget, FString Text)
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
		if (TextBlock != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("SetText: Cast widget %d as TextBlock..."), Widget->GetUniqueID());
			TextBlock->SetText(FText::FromString(Text));
			return true;
		}

		URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget);
		if (RichTextBlock != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("SetText: Cast widget %d as RichTextBlock..."), Widget->GetUniqueID());
			RichTextBlock->SetText(FText::FromString(Text));
			return true;
		}

		UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(Widget);
		if (EditableTextBox != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("SetText: Cast widget %d as EditableTextBox..."), Widget->GetUniqueID());
			EditableTextBox->SetText(FText::FromString(Text));
			return true;
		}

		UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(Widget);
		if (MultiLineEditableTextBox != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("SetText: Cast widget %d as MultiLineEditableTextBox..."), Widget->GetUniqueID());
			MultiLineEditableTextBox->SetText(FText::FromString(Text));
			return true;
		}
		return false;
	}


	bool FUIService::IsViewportInited = false;
	int32 FUIService::SurfaceViewWidth = 0;
	int32 FUIService::SurfaceViewHeight = 0;
	float FUIService::WidthScale = -1.0f;
	float FUIService::HeightScale = -1.0f;
	float FUIService::ViewportScale = -1.0f;


	bool FUIService::InitViewport()
	{
		if (IsViewportInited)
		{
			return true;
		}

#ifdef __ANDROID__
		IsInited = InitViewportAndroid();
#endif
		if (!IsViewportInited)
		{
			IsViewportInited = InitViewportDefault();
		}
		if (GEngine != nullptr)
		{
			if (!IsViewportInited)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UnrealAutomator UIService failed to initialize viewport!"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("UnrealAutomator UIService initialized viewport!"));
			}
		}
		return IsViewportInited;
	}
	
	bool FUIService::InitViewportDefault()
	{
		if (!IsViewportEnabledInEngine())
		{
			UE_LOG(UALog, Error, TEXT("Failed to init default viewport scale!!!"));
			return false;
		}
		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		const UUserInterfaceSettings* setting = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());
		if (setting != nullptr) {
			ViewportScale = setting->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

			if (ViewportScale <= 0.0) {
				UE_LOG(UALog, Error, TEXT("ViewportScale = %f,invaild"), ViewportScale);
				return false;
			}
			WidthScale = ViewportScale;
			HeightScale = ViewportScale;
			SurfaceViewWidth = GSystemResolution.ResX / ViewportScale;
			SurfaceViewHeight = GSystemResolution.ResY / ViewportScale;
			UE_LOG(UALog, Log, TEXT("Screen(GSystemResolution) with scale %f, size width= %f,height=%f"), ViewportScale, GSystemResolution.ResX / ViewportScale, GSystemResolution.ResY / ViewportScale);
		}
		return true;
	}

	bool FUIService::InitViewportAndroid()
	{
#ifdef  __ANDROID__
		if (!IsViewportEnabledInEngine())
		{
			UE_LOG(UALog, Error, TEXT("Failed to init Android viewport scale!!!"));
			Inited = false;
			return false;
		}
		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

		if (SurfaceViewWidth != 0.0f && SurfaceViewHeight != 0.0f) {
			return true;
		}
		void* NativeWindow = FAndroidWindow::GetHardwareWindow();

		FAndroidWindow::CalculateSurfaceSize(NativeWindow, SurfaceViewWidth, SurfaceViewHeight);
		// TODO: need error handler
		if (SurfaceViewWidth == 0.0f)
		{
			UE_LOG(UALog, Error, TEXT("SurfaceWidth error = 0.0 "));
			SurfaceViewWidth = ViewportSize.X;
		}
		if (SurfaceViewHeight == 0.0f)
		{
			UE_LOG(UALog, Error, TEXT("SurfaceViewHeight error = 0.0 "));
			SurfaceViewHeight = ViewportSize.Y;
		}
		WidthScale = ViewportSize.X / SurfaceViewWidth;
		HeightScale = ViewportSize.Y / SurfaceViewHeight;
		UE_LOG(LogTemp, Log, TEXT("Surfaceview WidthScale=%f ,HeightScale=%f,SurfaceViewWidth = %d,SurfaceViewHeight=%d"), WidthScale, HeightScale, SurfaceViewWidth, SurfaceViewHeight);
		return true;
#else
		return false;
#endif
	}

	bool FUIService::IsViewportEnabledInEngine()
	{
		if (GEngine == nullptr || GEngine->GameViewport == nullptr || GEngine->GameViewport->Viewport == nullptr)
		{
			UE_LOG(UALog, Error, TEXT("No GEngine Viewport!!!"));
			return false;
		}
		return true;
	}
}