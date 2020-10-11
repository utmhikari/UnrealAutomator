#pragma once


#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FSceneHandler
	{
	public:
		/**
		 * Get current map info
		 */
		static TUniquePtr<FHttpServerResponse> GetCurrentMapInfo(const FHttpServerRequest& Request);


	};
}

