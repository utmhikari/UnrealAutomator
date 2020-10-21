#pragma once

#include "CoreMinimal.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"


/**
 * common-used utils
 */
class FCommonUtil
{
public:
	// bool to string
	static FString BoolToString(bool Value);

	// Get current time, format as string
	static FString GetCurrentTime();

	// Json parse string to object
	static TSharedPtr<FJsonObject> JsonParse(FString Str);	
};

