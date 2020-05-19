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
	/**
	 * Start server
	 */
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

	/**
	 * Stop server
	 */
	void FWebServer::Stop()
	{
		UE_LOG(UALog, Log, TEXT("Stopping UnrealAutomator Server..."));
		auto HttpServerModule = &FHttpServerModule::Get();
		HttpServerModule->StopAllListeners();
	}

	/**
	 * Bind routers with handlers
	 */
	void FWebServer::BindRouters(TSharedPtr<IHttpRouter> HttpRouter)
	{
		// health check
		HttpRouter->BindRoute(FHttpPath(TEXT("/health")), EHttpServerRequestVerbs::VERB_GET, FWebUtil::CreateHandler(&FBaseHandler::HealthCheck));

		/* ====================== Player Handler ==================== */
		HttpRouter->BindRoute(FHttpPath(TEXT("/player/location")), EHttpServerRequestVerbs::VERB_GET, FWebUtil::CreateHandler(&FPlayerHandler::GetPlayerLocation));
	}
}