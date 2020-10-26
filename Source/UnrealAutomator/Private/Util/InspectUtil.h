#pragma once

#include "EngineMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"



/**
 * inspect utils
 * for deeper inspect into ue4 runtime
 */
class FInspectUtil
{
public:
	// Get object value of specific property name on its class definition
	static UProperty* GetPropertyByName(UObject* Object, FString Name);

	/**
	 * find class by string, see https://answers.unrealengine.com/questions/92651/get-blueprint-class-by-string-in-c.html
	 */
	static UClass* FindClass(const FString& Name);
};
