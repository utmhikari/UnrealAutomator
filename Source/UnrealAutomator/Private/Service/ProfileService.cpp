#include "Service/ProfileService.h"
#include "Kismet/GameplayStatics.h"
#include "UALog.h"
#include "Engine.h"
#include "Util/CommonUtil.h"
#include "Runtime/Core/Public/HAL/LowLevelMemTracker.h"


/* see GenericPlatformMemory.cpp */
TSharedPtr<FJsonObject> FProfileService::GetPlatformMemoryStats()
{
	FString PlatformName = UGameplayStatics::GetPlatformName();
#if defined(UA_PROFILE_SERVICE_OS_UNKNOWN)
	UE_LOG(UALog, Warning, TEXT("Failed to get platform memory stats! Unknown OS %s!"), *PlatformName);
	return nullptr;
#else
	FPlatformMemoryStats MemoryStats;
#if defined(UA_PROFILE_SERVICE_OS_WINDOWS)
	MemoryStats = FWindowsPlatformMemory::GetStats();
#elif defined(UA_PROFILE_SERVICE_OS_ANDROID)
	MemoryStats = FAndroidPlatformMemory::GetStats();
#elif defined(UA_PROFILE_SERVICE_OS_IOS)
	MemoryStats = FIOSPlatformMemory::GetStats();
#else
	UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot get %s memory stats! Definition lost!"), *PlatformName);
	MemoryStats = FPlatformMemoryStats();
#endif
	
	TSharedPtr<FJsonObject> MemoryStatsJson = MakeShareable(new FJsonObject());
	MemoryStatsJson->SetNumberField(TEXT("TotalPhysical"), MemoryStats.TotalPhysical);
	MemoryStatsJson->SetNumberField(TEXT("TotalVirtual"), MemoryStats.TotalVirtual);
	MemoryStatsJson->SetNumberField(TEXT("PageSize"), MemoryStats.PageSize);
	MemoryStatsJson->SetNumberField(TEXT("TotalPhysicalGB"), MemoryStats.TotalPhysicalGB);
	MemoryStatsJson->SetNumberField(TEXT("AvailablePhysical"), MemoryStats.AvailablePhysical);
	MemoryStatsJson->SetNumberField(TEXT("AvailableVirtual"), MemoryStats.AvailableVirtual);
	MemoryStatsJson->SetNumberField(TEXT("UsedPhysical"), MemoryStats.UsedPhysical);
	MemoryStatsJson->SetNumberField(TEXT("PeakUsedPhysical"), MemoryStats.PeakUsedPhysical);
	MemoryStatsJson->SetNumberField(TEXT("UsedVirtual"), MemoryStats.UsedVirtual);
	MemoryStatsJson->SetNumberField(TEXT("PeakUsedVirtual"), MemoryStats.PeakUsedVirtual);
	
	UE_LOG(LogUnrealAutomator, Log, TEXT("Get %s memory stats: %s"), *PlatformName, *FCommonUtil::JsonStringify(MemoryStatsJson));
	return MemoryStatsJson;
#endif
}

/* see UEngine::RenderStatFPS */
TSharedPtr<FJsonObject> FProfileService::GetFPSStats()
{
	TSharedPtr<FJsonObject> FPSStatsJson = MakeShareable(new FJsonObject());
	
	extern ENGINE_API float GAverageFPS;
	extern ENGINE_API float GAverageMS;

	FPSStatsJson->SetNumberField(TEXT("AvgFPS"), GAverageFPS);
	FPSStatsJson->SetNumberField(TEXT("AvgMS"), GAverageMS);

#if RHI_RAYTRACING
	extern ENGINE_API float GAveragePathTracedMRays;
	if (GAveragePathTracedMRays > 0.0)
	{
		FPSStatsJson->SetNumberField(TEXT("AvgPathTracedMRaysPerFrame"), GAveragePathTracedMRays);
		FPSStatsJson->SetNumberField(TEXT("AvgPathTracedMRaysPerSecond"), GAveragePathTracedMRays * GAverageFPS);
	}
#endif

	return FPSStatsJson;
}

/**
 * See HAL/LowLevelMemTracker.h
 * If detailed info is needed, a proper solution is to make vars/functions explicitly in LLM.h/.cpp
 */
TSharedPtr<FJsonObject> FProfileService::GetLLMStats()
{
#if ENABLE_LOW_LEVEL_MEM_TRACKER && STATS
	FLowLevelMemTracker& LLM = FLowLevelMemTracker::Get();

	// get all tags
	TArray<const TCHAR*> LLMTagNames;
	TArray<ELLMTag> LLMTags;
	TArray<FName> LLMTagStatNames;
	TArray<FName> LLMTagStatGroupNames;
	for (uint64 i = 0; i < (int32)ELLMTag::GenericTagCount; ++i)
	{
		const TCHAR* TagName = LLM.FindTagName(i);
		if (TagName != nullptr && !FString(TagName).Equals(TEXT("?")))
		{
			ELLMTag Tag = static_cast<ELLMTag>(i);
			LLMTags.Push(Tag);
			LLMTagNames.Push(TagName);
			/*LLMTagStatNames.Push(LLMGetTagStat(Tag));
			LLMTagStatGroupNames.Push(LLMGetTagStatGroup(Tag));*/
		}
	}
	for (uint64 i = LLM_CUSTOM_TAG_START; i <= LLM_CUSTOM_TAG_END; i++)
	{
		const TCHAR* TagName = LLM.FindTagName(i);
		if (TagName != nullptr && !FString(TagName).Equals(TEXT("?")))
		{
			ELLMTag Tag = static_cast<ELLMTag>(i);
			LLMTags.Push(Tag);
			LLMTagNames.Push(TagName);
			/*LLMTagStatNames.Push(LLMGetTagStat(Tag));
			LLMTagStatGroupNames.Push(LLMGetTagStatGroup(Tag));*/
		}
	}
	int32 NumTags = LLMTags.Num();
	UE_LOG(LogUnrealAutomator, Log, TEXT("Found %d LLM Tags!"), NumTags);
	if (NumTags == 0)
	{
		return nullptr;
	}

	// update stats
	LLM.UpdateStatsPerFrame();

	// gather amount
	TArray<TSharedPtr<FJsonValue>> LLMStatsArray;
	for (int32 i = 0; i < LLMTags.Num(); i++)
	{
		TSharedPtr<FJsonObject> TagAmountJson = MakeShareable(new FJsonObject());
		TagAmountJson->SetNumberField(TEXT("Amount"), LLM.GetTagAmountForTracker(ELLMTracker::Default, LLMTags[i]));
		TagAmountJson->SetStringField(TEXT("TagName"), LLMTagNames[i]);
		TagAmountJson->SetNumberField(TEXT("TagNum"), static_cast<uint64>(LLMTags[i]));
		/*TagAmountJson->SetStringField(TEXT("StatName"), LLMTagStatNames[i].ToString());
		TagAmountJson->SetStringField(TEXT("StatGroupName"), LLMTagStatGroupNames[i].ToString());*/
		LLMStatsArray.Push(MakeShareable(new FJsonValueObject(TagAmountJson)));
	}
	
	// make json
	TSharedPtr<FJsonObject> LLMStats = MakeShareable(new FJsonObject());
	LLMStats->SetArrayField(TEXT("Data"), LLMStatsArray);
	LLMStats->SetNumberField(TEXT("Count"), LLMStatsArray.Num());
	return LLMStats;
#else
	UE_LOG(LogUnrealAutomator, WARNING, TEXT("Cannot get LLM stats! STATS or ENABLE_LOW_LEVEL_MEM_TRACKER macro not enabled!"));
	return nullptr;
#endif
}


TSharedPtr<FJsonObject> FProfileService::GetCPUStats()
{
	FString PlatformName = UGameplayStatics::GetPlatformName();
#if defined(UA_PROFILE_SERVICE_OS_UNKNOWN)
	UE_LOG(UALog, Warning, TEXT("Failed to get platform CPU stats! Unknown OS %s!"), *PlatformName);
	return nullptr;
#else
	FCPUTime CPUTime(0.0, 0.0);
#if defined(UA_PROFILE_SERVICE_OS_WINDOWS)
	CPUTime = FWindowsPlatformTime::GetCPUTime();
#elif defined(UA_PROFILE_SERVICE_OS_ANDROID)
	CPUTime = FAndroidTime::GetCPUTime();
#elif defined(UA_PROFILE_SERVICE_OS_IOS)
	CPUTime = FApplePlatformTime::GetCPUTime();
#else
	UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot get %s CPU stats! Definition lost!"), *PlatformName);
#endif
	TSharedPtr<FJsonObject> CPUStatsJson = MakeShareable(new FJsonObject());
	CPUStatsJson->SetNumberField(TEXT("Pct"), CPUTime.CPUTimePct);
	CPUStatsJson->SetNumberField(TEXT("PctRelative"), CPUTime.CPUTimePctRelative);
	UE_LOG(LogUnrealAutomator, Log, TEXT("Get %s CPU stats: %s"), *PlatformName, *FCommonUtil::JsonStringify(CPUStatsJson));
	return CPUStatsJson;
#endif
}
