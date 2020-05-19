#pragma once

#include "CoreMinimal.h"

namespace UnrealAutomator
{
	class FStringUtil
	{
	public:
		static FString ByteArrayToString(const TArray<uint8>& ByteArray);
	};
}
