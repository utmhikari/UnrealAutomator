#include "Service/BaseService.h"
#include "Log.h"
#include "Util/CommonUtil.h"


bool FBaseService::Screenshot(FString Name, bool bIsTimeSuffixEnabled)
{
	FString Filename;
	if (bIsTimeSuffixEnabled)
	{
		Filename = FString::Printf(TEXT("%s_%s"), *Name, *FCommonUtil::GetCurrentTime());
	}
	else
	{
		Filename = Name;
	}

	UE_LOG(LogUnrealAutomator, Log, TEXT("Screenshot to file %s"), *Filename);

	FString Filepath = FString::Printf(TEXT("%sScreenshots/%s/%s"), *FPaths::ProjectSavedDir(), *FString(FPlatformProperties::IniPlatformName()), *Filename);

	FScreenshotRequest::RequestScreenshot(Filepath, true, false);

	return true;
}

