#pragma once

#include "CoreMinimal.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"



class FCommonUtil
{
public:
	// Get current time, format as string
	static FString GetCurrentTime();

	// Json parse string to object
	static TSharedPtr<FJsonObject> JsonParse(FString Str);
};

