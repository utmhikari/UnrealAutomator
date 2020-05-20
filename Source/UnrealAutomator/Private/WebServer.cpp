#include "WebServer.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Log.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpPath.h"


// Handlers
#include "Handler/BaseHandler.h"
#include "Handler/PlayerHandler.h"

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

	void FWebServer::BindRouters(TSharedPtr<IHttpRouter> HttpRouter)
	{
		// UE4 uses Map<String, Handle> to store router bindings, so that bind different verbs to a same HTTP path is not supported

		/* ====================== Base Handler ==================== */

		// health check
		HttpRouter->BindRoute(FHttpPath(TEXT("/health")), EHttpServerRequestVerbs::VERB_GET, FWebUtil::CreateHandler(&FBaseHandler::HealthCheck));

		/* ====================== Player Handler ==================== */

		// get player location
		HttpRouter->BindRoute(FHttpPath(TEXT("/player/get_location")), EHttpServerRequestVerbs::VERB_GET, FWebUtil::CreateHandler(&FPlayerHandler::GetPlayerLocation)); 

		// set player location
		HttpRouter->BindRoute(FHttpPath(TEXT("/player/set_location")), EHttpServerRequestVerbs::VERB_PUT, FWebUtil::CreateHandler(&FPlayerHandler::SetPlayerLocation));

		// get player rotation
		HttpRouter->BindRoute(FHttpPath(TEXT("/player/get_rotation")), EHttpServerRequestVerbs::VERB_GET, FWebUtil::CreateHandler(&FPlayerHandler::GetPlayerRotation));

		// set player rotation
		HttpRouter->BindRoute(FHttpPath(TEXT("/player/set_rotation")), EHttpServerRequestVerbs::VERB_PUT, FWebUtil::CreateHandler(&FPlayerHandler::SetPlayerRotation));
	}
}