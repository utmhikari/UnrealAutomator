#include "Util/WebUtil.h"
#include "Util/CommonUtil.h"
#include "Log.h"
#include "Engine.h"
#include "Kismet/KismetStringLibrary.h"


/** ========================== Public Methods ======================= */

FHttpRouteHandle FWebUtil::BindRoute(const TSharedPtr<IHttpRouter>& HttpRouter, FString Path, const EHttpServerRequestVerbs& Verb, const FHttpResponser& HttpResponser)
{
	// VERB_NONE not supported!
	if (HttpRouter == nullptr || Verb == EHttpServerRequestVerbs::VERB_NONE)
	{
		return nullptr;
	}

	FString VerbString = GetHttpVerbStringFromEnum(Verb);

	// check if HTTP path is valid
	FHttpPath HttpPath(Path);
	if (!HttpPath.IsValidPath())
	{	
		UE_LOG(UALog, Warning, TEXT("Failed to bind HTTP router %s %s! Invalid path!"), *VerbString, *Path);
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				FString::Printf(TEXT("Failed to bind HTTP router %s %s! Invalid path!"), *VerbString, *Path));
		}
		return nullptr;
	}

	// bind router
	auto RouteHandle = HttpRouter->BindRoute(HttpPath, Verb, FWebUtil::CreateHandler(HttpResponser));
	if (RouteHandle == nullptr)
	{
		// only fail if path exists
		UE_LOG(UALog, Warning, TEXT("Failed to bind HTTP router %s %s! Path already exists!"), *VerbString, *Path);
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				FString::Printf(TEXT("Failed to bind HTTP router %s %s! Path already exists!"), *VerbString, *Path));
		}
		return nullptr;
	}

	// bind successfully
	UE_LOG(UALog, Log, TEXT("Bind HTTP router: %s %s"), *VerbString, *Path);
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
			FString::Printf(TEXT("Bind HTTP router: %s %s"), *VerbString, *Path));
	}
	return RouteHandle;
}

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
	bool bIsUTF8JsonContent = CheckRequestContent(Request);
	if (!bIsUTF8JsonContent)
	{
		UE_LOG(UALog, Warning, TEXT("caught request not in utf-8 application/json body content!"));
		return nullptr;
	}

	FString RequestBodyString = GetRequestStringBody(Request);

	TSharedPtr<FJsonObject> RequestBody = FCommonUtil::JsonParse(RequestBodyString);
	if (RequestBody == nullptr)
	{
		UE_LOG(UALog, Warning, TEXT("failed to parse request string to json: %s"), *RequestBodyString);
	}

	return RequestBody;
}

TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message)
{
	return JsonResponse(Data, Message, true, static_cast<int32>(EHttpResponseCode::Success));
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
	if (Code == static_cast<int32>(EHttpResponseCode::Success))
	{
		Code = static_cast<int32>(EHttpResponseCode::Error);
	}
	return JsonResponse(Data, Message, false, Code);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message)
{
	if (Data == nullptr)
	{
		Data = MakeShareable(new FJsonObject());
	}
	return ErrorResponse(Data, Message, static_cast<int32>(EHttpResponseCode::Error));
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message, int32 Code)
{
	return ErrorResponse(MakeShareable(new FJsonObject()), Message, Code);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message)
{
	return ErrorResponse(MakeShareable(new FJsonObject()), Message, static_cast<int32>(EHttpResponseCode::Error));
}

/** ========================== Private Methods ======================= */

FString FWebUtil::GetHttpVerbStringFromEnum(const EHttpServerRequestVerbs& Verb)
{
	switch (Verb)
	{
	case EHttpServerRequestVerbs::VERB_GET:
		return TEXT("GET");
	case EHttpServerRequestVerbs::VERB_POST:
		return TEXT("POST");
	case EHttpServerRequestVerbs::VERB_PUT:
		return TEXT("PUT");
	case EHttpServerRequestVerbs::VERB_DELETE:
		return TEXT("DELETE");
	case EHttpServerRequestVerbs::VERB_PATCH:
		return TEXT("PATCH");
	case EHttpServerRequestVerbs::VERB_OPTIONS:
		return TEXT("OPTIONS");
	default:
		return TEXT("UNKNOWN_VERB");
	}
}

TUniquePtr<FHttpServerResponse> FWebUtil::JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool bSuccess, int32 Code)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetObjectField(TEXT("data"), Data);
	JsonObject->SetStringField(TEXT("message"), Message);
	JsonObject->SetBoolField(TEXT("success"), bSuccess);
	JsonObject->SetNumberField(TEXT("code"), (double)Code);
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return FHttpServerResponse::Create(JsonString, TEXT("application/json"));
}

	
FString FWebUtil::GetRequestStringBody(const FHttpServerRequest& Request)
{
	// Body to utf8 string, should be called after CheckRequestContent(Request, bIsCheckUTF8 = true)
	TArray<uint8> RequestBodyBytes = Request.Body;
	FString RequestBodyString = FString(UTF8_TO_TCHAR(RequestBodyBytes.GetData()));
	UE_LOG(UALog, Log, TEXT("Request string body: %s"), *RequestBodyString);
	return RequestBodyString;
}

bool FWebUtil::CheckRequestContent(const FHttpServerRequest& Request, bool bIsUTF8Checked, bool bIsJsonChecked)
{
	bool bIsUTF8Valid = !bIsUTF8Checked;
	bool bIsJsonValid = !bIsJsonChecked;
	for (auto& HeaderElem : Request.Headers)
	{
		auto LowerKey = HeaderElem.Key.ToLower();
		if (LowerKey == TEXT("content-type"))
		{
			for (auto& Value : HeaderElem.Value)
			{
				auto LowerValue = Value.ToLower();
				if (bIsUTF8Checked)
				{
					if (LowerValue == (TEXT("charset=utf-8")))
					{
						bIsUTF8Valid = true;
					}
				}
				if (bIsJsonChecked)
				{
					if (LowerValue == (TEXT("application/json")) || LowerValue == (TEXT("text/json")))
					{	
						bIsJsonValid = true;
					}
				}
			}
		}
#if WITH_EDITOR
		auto Value = FString::Join(HeaderElem.Value, TEXT(";"));
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
	// TODO: Get HTTP Request Path
	UE_LOG(UALog, Verbose, TEXT("Checked request content %s %s: (utf-8: %s, json: %s)"),
		*GetHttpVerbStringFromEnum(Request.Verb),
		*Request.RelativePath.GetPath(),
		*UKismetStringLibrary::Conv_BoolToString(bIsUTF8Valid),
		*UKismetStringLibrary::Conv_BoolToString(bIsJsonValid));
	return bIsUTF8Valid && bIsJsonValid;
}
