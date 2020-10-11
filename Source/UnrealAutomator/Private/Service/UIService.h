#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"


namespace UnrealAutomator
{
	/**
	 * Currently a copy of GAutomator
	 * View https://github.com/Tencent/GAutomator/tree/master/GAutomatorSdk/UnrealEngine/GAutomatorSdk/Plugins/GAutomator for details
	 */
	class FUIService
	{
	public:
		/**
		 * Screenshot
		 */
		static bool Screenshot(FString Name = TEXT("UA_Screenshot"), bool bEnableSuffix = true);

		/**
		 * Get widget tree in json format
		 */
		static TSharedPtr<FJsonObject> GetWidgetTreeJson();

	private:
		/**
		 * Traverse widget and fill the json parent
		 */
		static void TraverseWidget(UWidget* Widget, const TSharedPtr<FJsonObject>& Parent);

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
		 * viewport
		 */
		static bool IsViewportInited;
		static int32 SurfaceViewWidth;
		static int32 SurfaceViewHeight;
		static float WidthScale;
		static float HeightScale;
		static float ViewportScale;

		static bool InitViewport();
		static bool InitViewportDefault();
		static bool InitViewportAndroid();
		static bool IsViewportEnabledInEngine();
	};
}