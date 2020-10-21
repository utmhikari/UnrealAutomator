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
#include "Handler/CommandHandler.h"
#include "Handler/SceneHandler.h"



void FWebServer::Start(uint32 Port)
{
	auto HttpServerModule = &FHttpServerModule::Get();
	UE_LOG(LogUnrealAutomator, Log, TEXT("Starting UnrealAutomator server at port %d..."), Port);
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule->GetHttpRouter(Port);
	BindRouters(HttpRouter);
	// Start Listeners
	HttpServerModule->StartAllListeners();
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("UnrealAutomator server started at port %d"), Port));
	}
}

void FWebServer::Stop()
{
	UE_LOG(LogUnrealAutomator, Log, TEXT("Stopping UnrealAutomator Server..."));
	auto HttpServerModule = &FHttpServerModule::Get();
	HttpServerModule->StopAllListeners();
}

void FWebServer::BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter)
{
	// UE4 uses Map<String, Handle> to store router bindings
	// bind different verbs to a same HTTP path is not supported

	/* ====================== Base Handler ==================== */

	// health check
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/base/health"), EHttpServerRequestVerbs::VERB_GET, FBaseHandler::HealthCheck);

	// screenshot
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/base/screenshot"), EHttpServerRequestVerbs::VERB_GET, &FBaseHandler::Screenshot);

	/* ====================== Player Handler ==================== */

	// get player location
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/player/info"), EHttpServerRequestVerbs::VERB_GET, &FPlayerHandler::GetPlayerInfo);

	// set player location
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/player/location"), EHttpServerRequestVerbs::VERB_PUT, &FPlayerHandler::SetPlayerLocation);

	// set player rotation
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/player/rotation"), EHttpServerRequestVerbs::VERB_PUT, &FPlayerHandler::SetPlayerRotation);

	/* ====================== UI Handler ==================== */

	// get widget tree
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/ui/tree"), EHttpServerRequestVerbs::VERB_GET, &FUIHandler::GetWidgetTree);

	// get widgets on query
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/ui/widget"), EHttpServerRequestVerbs::VERB_POST, &FUIHandler::GetWidget);

	// call widget callback
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/ui/event"), EHttpServerRequestVerbs::VERB_POST, &FUIHandler::InvokeWidgetEvent);


	/* ====================== Command Handler ==================== */

	// execute unreal command
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/command/unreal"), EHttpServerRequestVerbs::VERB_POST, &FCommandHandler::ExecuteUECommand);

	// execute gm command
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/command/gm"), EHttpServerRequestVerbs::VERB_POST, &FCommandHandler::ExecuteGMCommand);

	/* ====================== Scene Handler ==================== */

	// get current map info
	FWebUtil::BindRoute(HttpRouter, TEXT("/v1/scene/map/current"), EHttpServerRequestVerbs::VERB_GET, &FSceneHandler::GetCurrentMapInfo);
}

