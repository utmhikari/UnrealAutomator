#pragma once

#include "CoreMinimal.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"

// profile stats may depend on platform itself
#if defined(__ANDROID__)
#define UA_PROFILE_SERVICE_OS_ANDROID 1
#include "Runtime/Core/Public/Android/AndroidPlatformTime.h"
#include "Runtime/Core/Public/Android/AndroidPlatformMemory.h"
#elif defined(__IOS__)
#define UA_PROFILE_SERVICE_OS_IOS 1
#include "Runtime/Core/Public/IOS/IOSPlatformTime.h"
#include "Runtime/Core/Public/IOS/IOSPlatformMemory.h"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define UA_PROFILE_SERVICE_OS_WINDOWS 1
#include "Runtime/Core/Public/Windows/WindowsPlatformTime.h"
#include "Runtime/Core/Public/Windows/WindowsPlatformMemory.h"
#else
#define UA_PROFILE_SERVICE_OS_UNKNOWN 1
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMemory.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformTime.h"
#endif


class FProfileService
{
public:
	/**
	 * get platform memory stats
	 */
	static TSharedPtr<FJsonObject> GetPlatformMemoryStats();

	/**
	 * get fps stats
	 */
	static TSharedPtr<FJsonObject> GetFPSStats();

	/**
	 * get LLM stats
	 */
	static TSharedPtr<FJsonObject> GetLLMStats();

	/**
	 * get cpu stats
	 */
	static TSharedPtr<FJsonObject> GetCPUStats();

};
