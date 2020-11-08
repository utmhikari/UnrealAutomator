#include "Handler/ProfileHandler.h"
#include "UALog.h"
#include "Util/WebUtil.h"
#include "Service/ProfileService.h"


TUniquePtr<FHttpServerResponse> FProfileHandler::GetProfileStats(const FHttpServerRequest& Request)
{
	TSharedPtr<FJsonObject> ResponseBody = MakeShareable(new FJsonObject());

	/* Memory */
	TSharedPtr<FJsonObject> MemoryStats = MakeShareable(new FJsonObject());

	// platform memory
	TSharedPtr<FJsonObject> PlatformMemoryStats = FProfileService::GetPlatformMemoryStats();
	MemoryStats->SetObjectField(TEXT("Platform"), PlatformMemoryStats);

	// LLM
	TSharedPtr<FJsonObject> LLMStats = FProfileService::GetLLMStats();
	MemoryStats->SetObjectField(TEXT("LLM"), LLMStats);

	ResponseBody->SetObjectField(TEXT("Memory"), MemoryStats);

	/* FPS */
	TSharedPtr<FJsonObject> FPSStats = FProfileService::GetFPSStats();
	ResponseBody->SetObjectField(TEXT("FPS"), FPSStats);

	/* CPU */
	TSharedPtr<FJsonObject> CPUStats = FProfileService::GetCPUStats();
	ResponseBody->SetObjectField(TEXT("CPU"), CPUStats);

	return FWebUtil::SuccessResponse(ResponseBody);

}

