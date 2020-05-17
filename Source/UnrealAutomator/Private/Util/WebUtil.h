#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerResponse.h"

namespace UnrealAutomator
{

	class FWebUtil
	{
	public:
		static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message);

		static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data);

		static TUniquePtr<FHttpServerResponse> SuccessResponse(FString Message);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message, int32 Code);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message);

	private:
		static TUniquePtr<FHttpServerResponse> JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool Success, int32 Code);
		
		static const int32 SUCCESS_CODE = 0;
		static const int32 DEFAULT_ERROR_CODE = -1;
	};
}