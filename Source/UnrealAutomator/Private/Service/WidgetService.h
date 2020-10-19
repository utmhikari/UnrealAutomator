#pragma once


#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"


/**
 * widget service
 * downward upon UIService
 * may be used by other services or USTRUCTs
 * 
 * References:
 * (1) GAutomator
 * https://github.com/Tencent/GAutomator/tree/master/GAutomatorSdk/UnrealEngine/GAutomatorSdk/Plugins/GAutomator
 * (2) PocoSDK
 * https://github.com/AirtestProject/Poco-SDK/blob/master/Unreal/PocoSDK/Source/PocoSDK
 */
class FWidgetService
{
public:
	/**
	 * Get json object to describe the widget
	 */
	static TSharedPtr<FJsonObject> GetWidgetJson(UWidget* Widget);


	/**
	 * Get text of widget
	 */
	static const FString* GetWidgetText(UWidget* Widget);

	/**
	 * Set text of widget
	 */
	static bool SetWidgetText(UWidget* Widget, FString Text);

	/**
	 * Invoke widget event
	 */
	static bool InvokeWidgetEvent(UWidget* Widget, FString Event);

};