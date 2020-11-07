#include "Util/CommonUtil.h"
#include "Runtime/Json/Public/Serialization/JsonTypes.h"
#include "Runtime/Json/Public/Dom/JsonValue.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"


FString FCommonUtil::GetCurrentTime()
{
	FDateTime CurrentTime = FDateTime::Now();
	return FString::Printf(
		TEXT("%d%d%d_%d%d%d"),
		CurrentTime.GetYear(),
		CurrentTime.GetMonth(),
		CurrentTime.GetDay(),
		CurrentTime.GetHour(),
		CurrentTime.GetMinute(),
		CurrentTime.GetSecond());
}

TSharedPtr<FJsonObject> FCommonUtil::JsonParse(FString Str)
{
	// string to json
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Str);
	TSharedPtr<FJsonObject> JsonObject;
	bool bSuccess = FJsonSerializer::Deserialize(JsonReader, JsonObject);
	if (!bSuccess)
	{
		return nullptr;
	}
	return JsonObject;
}

FString FCommonUtil::JsonStringify(TSharedPtr<FJsonObject> JsonObject)
{
	// json to string
	FString JsonStr = TEXT("");
	if (JsonObject != nullptr)
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	}
	return JsonStr;
}
