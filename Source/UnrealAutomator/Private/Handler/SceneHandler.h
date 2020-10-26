#pragma once


#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"


class FSceneHandler
{
public:
	/**
	 * Get current level info
	 */
	static TUniquePtr<FHttpServerResponse> GetCurrentLevelInfo(const FHttpServerRequest& Request);

	/**
	 * Get actor info
	 */
	static TUniquePtr<FHttpServerResponse> GetActorInfo(const FHttpServerRequest& Request);

	/**
	 * Get actors info
	 */
	static TUniquePtr<FHttpServerResponse> GetActorsInfo(const FHttpServerRequest& Request);
};


