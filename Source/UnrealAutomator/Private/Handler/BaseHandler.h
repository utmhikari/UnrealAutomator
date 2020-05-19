#pragma once

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FBaseHandler
	{
	public:
		static TUniquePtr<FHttpServerResponse> HealthCheck(const FHttpServerRequest& Request);


	};
}
