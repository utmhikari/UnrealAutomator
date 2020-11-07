#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"



class FProfileHandler
{
public:
	/**
	 * Get profile stats
	 */
	static TUniquePtr<FHttpServerResponse> GetProfileStats(const FHttpServerRequest& Request);
};