#pragma once

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FBaseHandler
	{
	public:
		static bool HealthCheck(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	};
}
