#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"


namespace UnrealAutomator
{
	/**
	 * Currently a copy of GAutomator
	 * View https://github.com/Tencent/GAutomator/tree/master/GAutomatorSdk/UnrealEngine/GAutomatorSdk/Plugins/GAutomator for details
	 */
	class FUIHandler
	{
	public:
		/**
		 * Get widget tree
		 */
		static TUniquePtr<FHttpServerResponse> GetWidgetTree(const FHttpServerRequest& Request);
	private:
		/**
		 * Get widget tree in json format
		 */
		static TSharedPtr<FJsonObject> GetWidgetTreeJson();

		/**
		 * Traverse widget and fill the json parent
		 */
		static void TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent);
	};
}