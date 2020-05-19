#include "Util/StringUtil.h"

namespace UnrealAutomator
{
	FString FStringUtil::ByteArrayToString(const TArray<uint8>& ByteArray)
	{
		return FString(UTF8_TO_TCHAR(ByteArray.GetData()));
	}
}
