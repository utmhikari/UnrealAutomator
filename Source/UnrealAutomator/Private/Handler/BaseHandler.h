#pragma once

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"


class FBaseHandler
{
public:
	/**
	 * Health Check
	 */
	static TUniquePtr<FHttpServerResponse> HealthCheck(const FHttpServerRequest& Request);

	/**
	 * Screenshot
	 */
	static TUniquePtr<FHttpServerResponse> Screenshot(const FHttpServerRequest& Request);
};

