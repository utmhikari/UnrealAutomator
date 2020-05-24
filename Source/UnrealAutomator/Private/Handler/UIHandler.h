#pragma once

#include "CoreMinimal.h"


namespace UnrealAutomator
{
	class FUIHandler
	{
	public:
		/**
		 * Get widget tree
		 */
		static TUniquePtr<FHttpServerResponse> GetWidgetTree(const FHttpServerRequest& Request);
	};
}