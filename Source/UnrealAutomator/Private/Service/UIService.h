#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Model/UIModel.h"



/**
 * UI Service
 * Act as agent of widget service
 */
class FUIService
{
public:
	/**
	 * Get widget tree in json format
	 */
	static TSharedPtr<FJsonObject> GetWidgetTreeJson();

	/**
	 * Find first specific widget on query
	 */
	static UWidget* FindWidget(FUIWidgetQuery Query);

private:
	/**
	 * Find first specific widget that matches widget query
	 */
	static UWidget* FindWidget(UWidget* Root, FUIWidgetQuery Query);

	/**
	 * Traverse widget and fill the json parent
	 */
	static void TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent);
};
