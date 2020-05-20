#include "Util/WebUtil.h"
#include "Log.h"
#include "Engine.h"
#include "Runtime/Json/Public/Serialization/JsonTypes.h"
#include "Runtime/Json/Public/Dom/JsonValue.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

namespace UnrealAutomator
{
	/** ========================== Public Methods ======================= */

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

	TSharedPtr<FJsonObject> FWebUtil::GetRequestJsonBody(const FHttpServerRequest& Request)
	{
		// check if content type is application/json
		bool IsUTF8JsonContent = IsUTF8JsonRequestContent(Request);
		if (!IsUTF8JsonContent)
		{
			UE_LOG(UALog, Warning, TEXT("caught request not in utf-8 application/json body content!"));
			return nullptr;
		}
		
		// body to utf8 string
		TArray<uint8> RequestBodyBytes = Request.Body;
		FString RequestBodyString = FString(UTF8_TO_TCHAR(RequestBodyBytes.GetData()));
#if WITH_EDITOR
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, RequestBodyString);
		}
#endif

		// string to json
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RequestBodyString);
		TSharedPtr<FJsonObject> RequestBody;
		if (!FJsonSerializer::Deserialize(JsonReader, RequestBody))
		{
			UE_LOG(UALog, Warning, TEXT("failed to parse request string to json: %s"), *RequestBodyString);
			return nullptr;
		}
		return RequestBody;
	}

	template <typename UStructType>
	static UStructType* FWebUtil::GetRequestUStructBody(const FHttpServerRequest& Request)
	{
		TSharedPtr<FJsonObject> JsonBody = FWebUtil::GetRequestJsonBody(Request);
		if (JsonBody == nullptr)
		{
			return nullptr;
		}
		UStructType* UStructBody;
		if (!FJsonObjectConverter::JsonObjectToUStruct(JsonBody, UStructBody))
		{
			UE_LOG(UALog, Warning, TEXT("failed to parse json body to ustruct!"))
			return nullptr;
		}
		return UStructBody;
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message)
	{
		return JsonResponse(Data, Message, true, SUCCESS_CODE);
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data)
	{
		return SuccessResponse(Data, TEXT(""));
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(FString Message)
	{
		return SuccessResponse(MakeShareable(new FJsonObject()), Message);
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code)
	{
		if (Code == SUCCESS_CODE)
		{
			Code = DEFAULT_ERROR_CODE;
		}
		return JsonResponse(Data, Message, false, Code);
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message)
	{
		return ErrorResponse(Data, Message, DEFAULT_ERROR_CODE);
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message, int32 Code)
	{
		return ErrorResponse(MakeShareable(new FJsonObject()), Message, Code);
	}

	TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message)
	{
		return ErrorResponse(MakeShareable(new FJsonObject()), Message, DEFAULT_ERROR_CODE);
	}

	/** ========================== Private Methods ======================= */

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

	bool FWebUtil::IsUTF8JsonRequestContent(const FHttpServerRequest& Request)
	{
		bool bIsUTF8JsonContent = false;
		for (auto& HeaderElem : Request.Headers)
		{
			if (HeaderElem.Key == TEXT("Content-type"))
			{
				for (auto& Value : HeaderElem.Value)
				{
					auto LowerValue = Value.ToLower();
					if (LowerValue.StartsWith(TEXT("charset=")) && LowerValue != TEXT("charset=utf-8"))
					{
						return false;
					}
					if (LowerValue == TEXT("application/json") || LowerValue == TEXT("text/json"))
					{
						bIsUTF8JsonContent = true;
					}
				}
			}
#if WITH_EDITOR
			auto Value = FString::Join(HeaderElem.Value, TEXT(","));
			FString Message;
			Message.Append(HeaderElem.Key);
			Message.AppendChars(TEXT(": "), 2);
			Message.Append(Value);
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, Message);
			}
#endif
		}
		return bIsUTF8JsonContent;
	}
}