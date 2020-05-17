#include "Util/WebUtil.h"

#include "Runtime/Json/Public/Serialization/JsonTypes.h"
#include "Runtime/Json/Public/Dom/JsonValue.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

namespace UnrealAutomator
{
	/** ========================== Public Methods ======================= */

	/**
	 * Success response (data & message)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message)
	{
		return JsonResponse(Data, Message, true, SUCCESS_CODE);
	}

	/**
	 * Success response (data only)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data)
	{
		return SuccessResponse(Data, TEXT(""));
	}

	/**
	 * Success response (message only)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(FString Message)
	{
		return SuccessResponse(MakeShareable(new FJsonObject()), Message);
	}

	/**
	 * Error response (data & message & code)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code)
	{
		if (Code == SUCCESS_CODE)
		{
			Code = DEFAULT_ERROR_CODE;
		}
		return JsonResponse(Data, Message, false, Code);
	}

	/**
	 * Error response (data & message)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message)
	{
		return ErrorResponse(Data, Message, DEFAULT_ERROR_CODE);
	}

	/**
	 * Error response (message & code)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message, int32 Code)
	{
		return ErrorResponse(MakeShareable(new FJsonObject()), Message, Code);
	}

	/**
	 * Error response (message only)
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message)
	{
		return ErrorResponse(MakeShareable(new FJsonObject()), Message, DEFAULT_ERROR_CODE);
	}

	/** ========================== Private Methods ======================= */

	/**
	 * Create json response from data, message, success status and user defined error code
	 */
	TUniquePtr<FHttpServerResponse> FWebUtil::JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool Success, int32 Code)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetObjectField(TEXT("data"), Data);
		JsonObject->SetStringField(TEXT("message"), Message);
		JsonObject->SetBoolField(TEXT("success"), Success);
		JsonObject->SetNumberField(TEXT("code"), (double)Code);
		FString JsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
		return FHttpServerResponse::Create(JsonString, TEXT("application/json"));
	}
}