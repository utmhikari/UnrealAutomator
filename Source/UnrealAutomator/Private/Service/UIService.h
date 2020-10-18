#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"



/**
 * UI Service
 */
class FUIService
{
public:
	/**
	 * Get widget tree in json format
	 */
	static TSharedPtr<FJsonObject> GetWidgetTreeJson();

	/**
	 * Get widget json
	 */
	static TSharedPtr<FJsonObject> GetWidgetJson();

	/**
	 * Call widget method
	 */
	static bool CallWidgetMethod();

private:
	/**
	 * Traverse widget and fill the json parent
	 */
	static void TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent);
};
