#pragma once


#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FPlayerHandler
	{
	public:
		static TUniquePtr<FHttpServerResponse> GetPlayerLocation(const FHttpServerRequest& Request);
	};
}