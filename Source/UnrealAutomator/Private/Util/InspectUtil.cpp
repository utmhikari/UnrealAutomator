#include "Util/InspectUtil.h"
#include "UALog.h"


UProperty* FInspectUtil::GetPropertyByName(UObject* Object, FString Name)
{
	if (Object == nullptr || Name.IsEmpty())
	{
		return nullptr;
	}

	UClass* ObjectClass = Object->GetClass();

	for (TFieldIterator<UProperty> It(ObjectClass); It; ++It)
	{
		UE_LOG(LogUnrealAutomator, Verbose, TEXT("Class: %s, Property: %s"), *ObjectClass->GetName(), *It->GetName());
		if (Name.Equals(It->GetName()))
		{
			UE_LOG(LogUnrealAutomator, Verbose, TEXT("Found property named %s in class %s!"), *Name, *ObjectClass->GetName());

			UProperty* Property = Cast<UProperty>(*It);
			return Property;
		}
	}

	UE_LOG(LogUnrealAutomator, Warning, TEXT("No property named %s in class %s..."), *Name, *ObjectClass->GetName());
	return nullptr;
}


UClass* FInspectUtil::FindClass(const FString& Name)
{
	UObject* ClassPackage = ANY_PACKAGE;

	if (UClass* Result = FindObject<UClass>(ClassPackage, *Name))
		return Result;

	if (UObjectRedirector* RenamedClassRedirector = FindObject<UObjectRedirector>(ClassPackage, *Name))
	{
		return CastChecked<UClass>(RenamedClassRedirector->DestinationObject);
	}
		
	// maybe not loaded
	if (UClass* Result = StaticLoadClass(UObject::StaticClass(), nullptr, *Name, nullptr, LOAD_None, nullptr))
	{
		return Result;
	}

	UE_LOG(LogUnrealAutomator, Warning, TEXT("Cannot find class with name %s"), *Name);
	return nullptr;
}
