#pragma once

#include "CoreMinimal.h"

#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerResponse.h"

namespace UnrealAutomator
{
	typedef TFunction<TUniquePtr<FHttpServerResponse>(const FHttpServerRequest& Request)> FHttpResponser;

	class FWebUtil
	{
	public:	
		static FHttpRequestHandler CreateHandler(const FHttpResponser& HttpResponser);

		static TSharedPtr<FJsonObject> GetRequestBody(const FHttpServerRequest& Request);

		static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message);

		static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data);

		static TUniquePtr<FHttpServerResponse> SuccessResponse(FString Message);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message, int32 Code);

		static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message);
	private:
		static const int32 SUCCESS_CODE = 0;
		static const int32 DEFAULT_ERROR_CODE = -1;

		static TUniquePtr<FHttpServerResponse> JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool Success, int32 Code);
	};
}