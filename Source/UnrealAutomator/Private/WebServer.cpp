#include "WebServer.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpPath.h"


// Handlers
#include "Handler/BaseHandler.h"
#include "Handler/PlayerHandler.h"
#include "Handler/UIHandler.h"


namespace UnrealAutomator
{	
	void FWebServer::Start(uint32 Port)
	{
		auto HttpServerModule = &FHttpServerModule::Get();
		UE_LOG(UALog, Log, TEXT("Starting UnrealAutomator Server..."));
		TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule->GetHttpRouter(Port);
		BindRouters(HttpRouter);
		// Start Listeners
		HttpServerModule->StartAllListeners();
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("UnrealAutomator Server Started"));
		}
	}

	void FWebServer::Stop()
	{
		UE_LOG(UALog, Log, TEXT("Stopping UnrealAutomator Server..."));
		auto HttpServerModule = &FHttpServerModule::Get();
		HttpServerModule->StopAllListeners();
	}

	void FWebServer::BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter)
	{
		// UE4 uses Map<String, Handle> to store router bindings, so that bind different verbs to a same HTTP path is not supported

		/* ====================== Base Handler ==================== */

		// health check
		FWebUtil::BindRoute(HttpRouter, TEXT("/health"), EHttpServerRequestVerbs::VERB_GET, FBaseHandler::HealthCheck);

		/* ====================== Player Handler ==================== */

		// get player location
		FWebUtil::BindRoute(HttpRouter, TEXT("/player/get_location"), EHttpServerRequestVerbs::VERB_GET, &FPlayerHandler::GetPlayerLocation);

		// set player location
		FWebUtil::BindRoute(HttpRouter, TEXT("/player/set_location"), EHttpServerRequestVerbs::VERB_PUT, &FPlayerHandler::SetPlayerLocation);

		// get player rotation
		FWebUtil::BindRoute(HttpRouter, TEXT("/player/get_rotation"), EHttpServerRequestVerbs::VERB_GET, &FPlayerHandler::GetPlayerRotation);

		// set player rotation
		FWebUtil::BindRoute(HttpRouter, TEXT("/player/set_rotation"), EHttpServerRequestVerbs::VERB_PUT, &FPlayerHandler::SetPlayerRotation);

		/* ====================== UI Handler ==================== */

		// widget 
		FWebUtil::BindRoute(HttpRouter, TEXT("/ui/widget_tree"), EHttpServerRequestVerbs::VERB_GET, &FUIHandler::GetWidgetTree);
	}
}