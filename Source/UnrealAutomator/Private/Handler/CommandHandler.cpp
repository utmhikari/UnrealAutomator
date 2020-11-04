#include "Handler/CommandHandler.h"
#include "Util/WebUtil.h"
#include "Engine.h"
#include "../Log.h"
#include "Service/PlayerService.h"


TUniquePtr<FHttpServerResponse> FCommandHandler::ExecuteUECommand(const FHttpServerRequest& Request)
{
	if (GEngine == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("GEngine not found!"));
	}
	TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
	FString Command;
	if (RequestBody == nullptr ||
		!RequestBody->TryGetStringField(TEXT("Cmd"), Command))
	{
		return FWebUtil::ErrorResponse(TEXT("command not found!"));
	}
	auto PlayerController = FPlayerService::GetPlayerController();
	if (PlayerController == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("player controller not found!"));
	}
	PlayerController->ConsoleCommand(Command);
	UE_LOG(LogUnrealAutomator, Log, TEXT("Executed UE command: %s"), *Command);
#if WITH_EDITOR
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Yellow,
			FString::Printf(TEXT("Executed UE command: %s"), *Command));
	}
#endif
	return FWebUtil::SuccessResponse(TEXT("Executed UE command!"));
}

TUniquePtr<FHttpServerResponse> FCommandHandler::ExecuteGMCommand(const FHttpServerRequest& Request)
{
	return FWebUtil::ErrorResponse(TEXT("Not implemented"));
}

TUniquePtr<FHttpServerResponse> FCommandHandler::ExecuteLuaCommand(const FHttpServerRequest& Request)
{
	TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
	FString Command = TEXT("");
	if (RequestBody == nullptr || !RequestBody->TryGetStringField(TEXT("Cmd"), Command) || Command.IsEmpty())
	{
		return FWebUtil::ErrorResponse(TEXT("lua command not found!"));
	}
	UE_LOG(LogUnrealAutomator, Log, TEXT("Exec lua command: %s"), *Command);

#if defined(UA_UNLUA_ENABLED)
	// UnLua
	if (!FUnLuaDelegates::ReportLuaCallError.IsBound())
	{
		FUnLuaDelegates::ReportLuaCallError.BindStatic(&UnLuaCallErrorReporter);
	}

	bool bSuccess = UnLua::RunChunk(UnLua::GetState(), TCHAR_TO_UTF8(*Command));
	if (!bSuccess)
	{
		FString Message = FString::Printf(TEXT("exec UnLua command failed! %s"), *UnLuaCallErrorMessage);
		return FWebUtil::ErrorResponse(Message);
	}
	else
	{
		return FWebUtil::SuccessResponse(TEXT("exec UnLua command successfully!"));
	}
#elif defined(UA_SLUA_ENABLED)
	// slua_unreal
	NS_SLUA::LuaState* MainState = NS_SLUA::LuaState::get();
	if (MainState == nullptr)
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to get main lua_State of slua_unreal!"));
	}
	// override doString, doBuffer, LuaVar::call, see LuaState.cpp
	bool bSuccess = false;
	const char* CommandStr = TCHAR_TO_UTF8(*Command);
	NS_SLUA::lua_State* L = MainState->getLuaState();
	NS_SLUA::AutoStack g(L);
	int errfunc = NS_SLUA::LuaState::pushErrorHandler(L);
	if (luaL_loadbuffer(L, CommandStr, strlen(CommandStr), CommandStr)) {
		const char* Err = lua_tostring(L, -1);
		NS_SLUA::Log::Error("Failed to load buffer on command %s! %s", CommandStr, Err);
		return FWebUtil::ErrorResponse(FString::Printf(TEXT("Failed to load buffer in slua_unreal lua_State! %s"), UTF8_TO_TCHAR(Err)));
	}
	NS_SLUA::LuaVar f(L, -1);
	if (!f.isFunction())
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to exec command in slua_unreal! LuaVar is not a function!"));
	}
	if (!f.isValid())
	{
		return FWebUtil::ErrorResponse(TEXT("Failed to call command in slua_unreal! LuaVar invalid!"));
	}
	f.callWithNArg(0);
	return FWebUtil::SuccessResponse(TEXT("exec slua_unreal command successfully!"));
#else
	UE_LOG(LogUnrealAutomator, Warning, TEXT("No module can be used to run lua command!"));
	return FWebUtil::ErrorResponse(TEXT("Not implemented"));
#endif
}


#if defined(UA_UNLUA_ENABLED)
int32 FCommandHandler::UnLuaCallErrorReporter(lua_State* L)
{
	int32 Type = lua_type(L, -1);
	if (Type == LUA_TSTRING)
	{
		const char* ErrorString = lua_tostring(L, -1);
		luaL_traceback(L, L, ErrorString, 1);
		ErrorString = lua_tostring(L, -1);
		UE_LOG(LogUnLua, Warning, TEXT("Lua error message: %s"), UTF8_TO_TCHAR(ErrorString));
		UnLuaCallErrorMessage = UTF8_TO_TCHAR(ErrorString);
	}
	else if (Type == LUA_TTABLE)
	{
		// multiple errors is possible
		int32 MessageIndex = 0;
		lua_pushnil(L);
		UnLuaCallErrorMessage = TEXT("");
		int32 ErrorCnt = 0;
		FString TmpMessage = TEXT("");
		while (lua_next(L, -2) != 0)
		{
			ErrorCnt++;
			const char* ErrorString = lua_tostring(L, -1);
			UE_LOG(LogUnLua, Warning, TEXT("Lua error message %d : %s"), MessageIndex++, UTF8_TO_TCHAR(ErrorString));
			TmpMessage += FString::Printf(TEXT("<%d> %s; "), ErrorCnt, UTF8_TO_TCHAR(ErrorString));
			lua_pop(L, 1);
		}
		UnLuaCallErrorMessage = MoveTemp(TmpMessage);
	}
	lua_pop(L, 1);
	return 0;
}
#endif
