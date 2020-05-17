#include "WebServer.h"
#include "Log.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "Runtime/Online/HTTPServer/Public/HttpPath.h"
#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"

// Handlers
#include "Handler/BaseHandler.h"

namespace UnrealAutomator
{
	FWebServer::FWebServer()
	{
		Port = DEFAULT_PORT;
		Init();
	}

	FWebServer::FWebServer(uint32 Port)
	{
		this->Port = Port;
		Init();
	}

	FWebServer::~FWebServer()
	{
		if (HttpServerModule != nullptr)
		{
			UE_LOG(UALog, Log, TEXT("Stopping UnrealAutomator Server..."));
			HttpServerModule->StopAllListeners();
		}
	}

	/**
	 * Initialize web server
	 */
	void FWebServer::Init()
	{
		HttpServerModule = &FHttpServerModule::Get();
		UE_LOG(UALog, Log, TEXT("Starting UnrealAutomator Server..."));
		BindRouters();
		// Start Listeners
		HttpServerModule->StartAllListeners();
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("UnrealAutomator Server Started"));
		}
	}

	/**
	 * bind routes with handlers
	 */
	void FWebServer::BindRouters()
	{
		TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule->GetHttpRouter(Port);

		// health check
		HttpRouter->BindRoute(
			FHttpPath(TEXT("/health")),
			EHttpServerRequestVerbs::VERB_GET,
			&FBaseHandler::HealthCheck);
	}
};
