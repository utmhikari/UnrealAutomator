#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"

namespace UnrealAutomator
{
	class FCommandHandler
	{
	public:
		/**
		 * Execute UE4 command
		 */
		static TUniquePtr<FHttpServerResponse> ExecuteUECommand(const FHttpServerRequest& Request);

		/**
		 * Execute GM command
		 */
		static TUniquePtr<FHttpServerResponse> ExecuteGMCommand(const FHttpServerRequest& Request);
	};
}
