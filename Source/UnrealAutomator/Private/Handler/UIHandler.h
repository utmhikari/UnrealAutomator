#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"


namespace UnrealAutomator
{
	class FUIHandler
	{
	public:
		/**
		 * Get widget tree
		 */
		static TUniquePtr<FHttpServerResponse> GetWidgetTree(const FHttpServerRequest& Request);

		/**
		 * Screenshot
		 */
		static TUniquePtr<FHttpServerResponse> Screenshot(const FHttpServerRequest& Request);
	};
}