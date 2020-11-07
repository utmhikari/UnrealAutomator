#include "Service/ViewportService.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine.h"
#include "UALog.h"

#ifdef __ANDROID__
#include "Android/AndroidWindow.h"
#endif


bool FViewportService::bIsInited = false;
int32 FViewportService::SurfaceViewWidth = 0;
int32 FViewportService::SurfaceViewHeight = 0;
float FViewportService::WidthScale = -1.0f;
float FViewportService::HeightScale = -1.0f;
float FViewportService::ViewportScale = -1.0f;


bool FViewportService::Init()
{
	if (bIsInited)
	{
		return true;
	}

#ifdef __ANDROID__
	bIsInited = InitAndroid();
#endif
	if (!bIsInited)
	{
		bIsInited = InitDefault();
	}
	if (GEngine != nullptr)
	{
		if (!bIsInited)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UnrealAutomator UIService failed to initialize viewport!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("UnrealAutomator UIService initialized viewport!"));
		}
	}
	return bIsInited;
}


bool FViewportService::Reset()
{
	bIsInited = false;
	return Init();
}


bool FViewportService::InitDefault()
{
	if (!IsViewportEnabledInEngine())
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("Failed to init default viewport scale!!!"));
		return false;
	}
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	const UUserInterfaceSettings* Setting = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass());

	if (Setting == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("Failed to load UserInterface settings!"));
		return false;
	}

	ViewportScale = Setting->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

	if (ViewportScale <= 0.0) {
		UE_LOG(LogUnrealAutomator, Error, TEXT("ViewportScale = %f,invaild"), ViewportScale);
		return false;
	}
	WidthScale = ViewportScale;
	HeightScale = ViewportScale;
	SurfaceViewWidth = GSystemResolution.ResX / ViewportScale;
	SurfaceViewHeight = GSystemResolution.ResY / ViewportScale;
	UE_LOG(LogUnrealAutomator, Log, TEXT("Screen(GSystemResolution) with ViewportScale: %f, SurfaceViewWidth: %f, SurfaceViewHeight: %f"), ViewportScale, SurfaceViewWidth, SurfaceViewHeight);
	return true;
}

bool FViewportService::InitAndroid()
{
#ifdef  __ANDROID__

	// check if viewport is enabled
	if (!IsViewportEnabledInEngine())
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("Failed to init Android viewport scale!!!"));
		return false;
	}

	// cache viewport size
	auto Viewport = GEngine->GameViewport->Viewport;
	const FVector2D ViewportSize = FVector2D(Viewport->GetSizeXY());
	UE_LOG(LogUnrealAutomator, Log, TEXT("Android viewport size X: %f, Y: %f..."), ViewportSize.X, ViewportSize.Y);

	// calculate surface size
	FAndroidWindow::CalculateSurfaceSize(SurfaceViewWidth, SurfaceViewHeight);
	
	// calculate scale and size
	// TODO: solve problem of black border
	if (SurfaceViewWidth == 0)
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("Android SurfaceWidth error = 0.0 "));
		SurfaceViewWidth = ViewportSize.X;
	}
	if (SurfaceViewHeight == 0)
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("Android SurfaceViewHeight error = 0.0 "));
		SurfaceViewHeight = ViewportSize.Y;
	}
	WidthScale = ViewportSize.X / SurfaceViewWidth;
	HeightScale = ViewportSize.Y / SurfaceViewHeight;
	UE_LOG(LogUnrealAutomator, Log, TEXT("Android Surfaceview WidthScale: %f, HeightScale: %f, SurfaceViewWidth: %d, SurfaceViewHeight: %d"), WidthScale, HeightScale, SurfaceViewWidth, SurfaceViewHeight);
	return true;
#else
	return false;
#endif
}


bool FViewportService::IsViewportEnabledInEngine()
{
	if (GEngine == nullptr || GEngine->GameViewport == nullptr || GEngine->GameViewport->Viewport == nullptr)
	{
		UE_LOG(LogUnrealAutomator, Error, TEXT("No GEngine Viewport!!!"));
		return false;
	}
	return true;
}
