#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"



class FBaseService
{
public:
	/**
	 * Screenshot
	 */
	static bool Screenshot(FString Name = TEXT("UA_Screenshot"), bool bIsTimeSuffixEnabled = true);
};

