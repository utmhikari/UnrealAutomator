#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"


// UnLua
#if defined(AUTO_UNLUA_STARTUP)
#define UA_UNLUA_ENABLED 1
#include "Unlua.h"
#include "UnLuaDelegates.h"
#endif


class FCommandHandler
{
public:
	/**
	 * Execute UE4 command
	 */
	static TUniquePtr<FHttpServerResponse> ExecuteUECommand(const FHttpServerRequest& Request);

	/**
	 * Execute GM command
	 */
	static TUniquePtr<FHttpServerResponse> ExecuteGMCommand(const FHttpServerRequest& Request);

	/**
	 * Execute Lua command
	 */
	static TUniquePtr<FHttpServerResponse> ExecuteLuaCommand(const FHttpServerRequest& Request);

private:
#if defined(UA_UNLUA_ENABLED)
	static FString UnLuaCallErrorMessage;

	/**
	 * Error Reporter of UnLua
	 */
	static int32 UnLuaCallErrorReporter(lua_State* L);
#endif
};

#if defined(UA_UNLUA_ENABLED)

FString FCommandHandler::UnLuaCallErrorMessage = TEXT("");

#endif

