#include "Util/CommonUtil.h"


namespace UnrealAutomator
{
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
}