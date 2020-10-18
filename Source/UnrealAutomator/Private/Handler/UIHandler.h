#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"



class FUIHandler
{
public:
	/**
	 * Get widget tree
	 */
	static TUniquePtr<FHttpServerResponse> GetWidgetTree(const FHttpServerRequest& Request);

	/**
	 * Get widget
	 */
	static TUniquePtr<FHttpServerResponse> GetWidget(const FHttpServerRequest& Request);

	/**
	 * Call widget callback
	 */
	static TUniquePtr<FHttpServerResponse> CallWidgetMethod(const FHttpServerRequest& Request);
};
