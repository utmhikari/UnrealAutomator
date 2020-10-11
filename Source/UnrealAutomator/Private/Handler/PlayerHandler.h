#pragma once


#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FPlayerHandler
	{
	public:
		/**
		 * get player info
		 */
		static TUniquePtr<FHttpServerResponse> GetPlayerInfo(const FHttpServerRequest& Request);

		/**
		 * set player location
		 */
		static TUniquePtr<FHttpServerResponse> SetPlayerLocation(const FHttpServerRequest& Request);

		/**
		 * set player rotation
		 */
		static TUniquePtr<FHttpServerResponse> SetPlayerRotation(const FHttpServerRequest& Request);
	};
}