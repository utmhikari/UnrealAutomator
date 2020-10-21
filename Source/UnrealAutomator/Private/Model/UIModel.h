#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UIModel.generated.h"


enum class EUIWidgetQueryMatchState : int8
{
	None = 0,
	Child,
	Parent,
	All,
};

/**
 * Query template for ui widget
 */
USTRUCT()
struct FUIWidgetQuery
{
	GENERATED_BODY()

public:
	UPROPERTY()
		int32 ID = 0;

	UPROPERTY()
		FString Name = TEXT("");

	UPROPERTY()
		FString Text = TEXT("");

	// properties below are relatively no need to be modified

	UPROPERTY()
		FString ClassName = TEXT("");

	UPROPERTY()
		bool bIsKeptParentClass = false;

	UPROPERTY()
		bool bIsNameAsKeyword = false;

	UPROPERTY()
		bool bIsTextAsKeyword = false;

	FUIWidgetQuery()
		: ID(0)
		, Name(TEXT(""))
		, Text(TEXT(""))
		, ClassName(TEXT(""))
		, bIsKeptParentClass(false)
		, bIsNameAsKeyword(false)
		, bIsTextAsKeyword(false)
	{}

	FUIWidgetQuery(int32 InID,
		FString InName,
		FString InText,
		FString InClassName,
		bool bInIsKeptParentClass,
		bool bInIsNameAsKeyword,
		bool bInIsTextAsKeyword)
		: ID(InID)
		, Name(InName)
		, Text(InText)
		, ClassName(InClassName)
		, bIsKeptParentClass(bInIsKeptParentClass)
		, bIsNameAsKeyword(bInIsNameAsKeyword)
		, bIsTextAsKeyword(bInIsTextAsKeyword)
	{}

	FString ToString();
	

	EUIWidgetQueryMatchState Match(UWidget* Widget,
		bool bIsDisabledIncluded = false,
		bool bIsInvisibleIncluded = false);

};
