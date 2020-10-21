#pragma once

#include "CoreMinimal.h"
#include "Log.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerRequest.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerResponse.h"
#include "Runtime/Online/HTTPServer/Public/HttpRouteHandle.h"
#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"

#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonTypes.h"
#include "Runtime/Json/Public/Dom/JsonValue.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"



/**
* HTTP responser function
*/
typedef TFunction<TUniquePtr<FHttpServerResponse>(const FHttpServerRequest& Request)> FHttpResponser;

enum class EHttpResponseCode : int32
{
	Error = -1,
	Success = 0,
};


class FWebUtil
{
public:	
	/**
	* Bind a route with handler
	*/
	static FHttpRouteHandle BindRoute(const TSharedPtr<IHttpRouter>& HttpRouter, FString Path, const EHttpServerRequestVerbs& Verb, const FHttpResponser& HttpResponser);

	/**
	* Create HTTP request handler (controller)
	* In UE4, invoke OnComplete and return false will cause crash
	* CreateHandler method is used to wrap the responser, in order to avoid the crash
	*/
	static FHttpRequestHandler CreateHandler(const FHttpResponser& HttpResponser);

	/**
	* Get request json body, parse TArray<uint8> to TSharedPtr<FJsonObject>
	*/
	static TSharedPtr<FJsonObject> GetRequestJsonBody(const FHttpServerRequest& Request);

	/**
	* Get Struct body (based on json body, the struct type should be UStruct)
	*/
	template <typename UStructType>
	static bool GetRequestUStructBody(const FHttpServerRequest& Request, UStructType* StructBody)
	{
		verifyf(StructBody != nullptr, TEXT("USTRUCT to be converted should not be null~"));

		TSharedPtr<FJsonObject> JsonBody = GetRequestJsonBody(Request);
		if (JsonBody == nullptr)
		{
			return false;
		}

		// extend/update struct with json values
		if (!FJsonObjectConverter::JsonObjectToUStruct<UStructType>(JsonBody.ToSharedRef(), StructBody, 0, 0))
		{
			UE_LOG(LogUnrealAutomator, Warning, TEXT("failed to parse json body to ustruct!"))
			return false;
		}

		if (StructBody == nullptr)
		{
			UE_LOG(LogUnrealAutomator, Warning, TEXT("cast to USTRUCT failed! struct ptr is still null!"));
			return false;
		}

		UE_LOG(LogUnrealAutomator, Log, TEXT("convert to USTRUCT successfully!"));

		return true;
	}

	/**
	 * Success response (data & message)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message);

	/**
	 * Success response (data only)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data);

	/**
	 * Success response (message only)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(FString Message);

	/**
	 * Error response (data & message & code)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code);

	/**
	 * Error response (data & message)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message);

	/**
	 * Error response (message & code)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message, int32 Code);

	/**
	 * Error response (message only)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message);

private:
	/**
	 * get verb string from enumerate (for logging use)
	 */
	static FString GetHttpVerbStringFromEnum(const EHttpServerRequestVerbs& Verb);

	/**
	 * Create json response from data, message, success status and user defined error code
	 */
	static TUniquePtr<FHttpServerResponse> JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool bSuccess, int32 Code);

	/**
	 * Get body string
	 */
	static FString GetRequestStringBody(const FHttpServerRequest& Request);

	/**
	 * Check request content on header
	 */
	static bool CheckRequestContent(const FHttpServerRequest& Request, bool bIsUTF8Checked = true, bool bIsJsonChecked = true);
};



