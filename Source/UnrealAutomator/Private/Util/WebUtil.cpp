#include "Util/WebUtil.h"
#include "Util/StringUtil.h"
#include "Log.h"
#include "Engine.h"
#include "Runtime/Json/Public/Serialization/JsonTypes.h"
#include "Runtime/Json/Public/Dom/JsonValue.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

namespace UnrealAutomator
{
	/** ========================== Public Methods ======================= */

	/**
	 * Create HTTP request handler (controller)
	 * In UE4, invoke OnComplete and return false will cause crash
	 * CreateHandler method is used to wrap the responser, in order to avoid the crash
	 */
	FHttpRequestHandler FWebUtil::CreateHandler(const FHttpResponser& HttpResponser)
	{
		return [HttpResponser](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			auto Response = HttpResponser(Request);
			if (Response == nullptr)
			{
				return false;
			}
			OnComplete(MoveTemp(Response));
			return true;
		};
	}

	/**
	 * Get request json body, parse TArray<uint8> to TSharedPtr<FJsonObject>
	 */
	TSharedPtr<FJsonObject> FWebUtil::GetRequestBody(const FHttpServerRequest& Request)
	{
		// TODO: check request header 
		if (GEngine != nullptr)
		{
			for (auto& HeaderElem : Request.Headers)
			{
				auto Value = FString::Join(HeaderElem.Value, TEXT(","));
				FString Message;
				Message.Append(HeaderElem.Key);
				Message.AppendChars(TEXT(": "), 2);
				Message.Append(Value);
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, Message);
			}
		}
		
		TArray<uint8> RequestBodyBytes = Request.Body;
		FString RequestBodyString = FStringUtil::ByteArrayToString(RequestBodyBytes);
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, RequestBodyString);
		}
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RequestBodyString);
		TSharedPtr<FJsonObject> JsonObject;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			return JsonObject;
		}
		return nullptr;
	}

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