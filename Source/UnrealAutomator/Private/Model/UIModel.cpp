#include "Model/UIModel.h"
#include "Service/WidgetService.h"
#include "Util/CommonUtil.h"
#include "Kismet/KismetStringLibrary.h"


FString FUIWidgetQuery::ToString()
{
	return FString::Printf(
		TEXT("ID: %d, Name: %s, Text: %s, ClassName: %s, bIsKeptParentClass: %s, bIsNameAsKeyword: %s, bIsTextAsKeyword: %s"),
		ID,
		*Name,
		*Text,
		*ClassName,
		*UKismetStringLibrary::Conv_BoolToString(bIsKeptParentClass),
		*UKismetStringLibrary::Conv_BoolToString(bIsNameAsKeyword),
		*UKismetStringLibrary::Conv_BoolToString(bIsTextAsKeyword)
	);
}


EUIWidgetQueryMatchState FUIWidgetQuery::Match(UWidget* Widget, bool bIsDisabledIncluded, bool bIsInvisibleIncluded)
{
	// init as All
	EUIWidgetQueryMatchState MatchState = EUIWidgetQueryMatchState::All;
		
	// check valid
	if (Widget == nullptr)
	{
		return EUIWidgetQueryMatchState::None;
	}

	if (!bIsDisabledIncluded && !Widget->GetIsEnabled())
	{
		return EUIWidgetQueryMatchState::None;
	}

	if (!bIsInvisibleIncluded && !Widget->IsVisible())
	{
		return EUIWidgetQueryMatchState::None;
	}

	// check class, only class name will consider parent/child relations
	bool bHasClassName = !this->ClassName.IsEmpty();
	if (bHasClassName)
	{
		if (!this->ClassName.Equals(Widget->GetClass()->GetName()))
		{
			if (!this->bIsKeptParentClass)
			{
				return EUIWidgetQueryMatchState::None;
			}
			else
			{
				// All -> Child
				MatchState = EUIWidgetQueryMatchState::Child;
			}
		}
	}
	else
	{
		// All -> Child
		MatchState = EUIWidgetQueryMatchState::Child;
	}

	// check empty
	if (this->ID == 0 && this->Name.IsEmpty() && this->Text.IsEmpty())
	{
		if (!bHasClassName || (this->bIsKeptParentClass && MatchState == EUIWidgetQueryMatchState::Child))
		{
			// 1. completely empty query
			// 2. class mismatches, others empty
			return EUIWidgetQueryMatchState::None;
		}
		else
		{
			// class matches yet others empty, this is the one and only
			return EUIWidgetQueryMatchState::All;
		}
	}

	// check ID
	if (this->ID != 0)
	{
		if (this->ID != Widget->GetUniqueID())
		{
			if (!bIsKeptParentClass || MatchState != EUIWidgetQueryMatchState::All)
			{
				// impossible to become a parent
				return EUIWidgetQueryMatchState::None;
			}
			else
			{
				// All -> Parent
				MatchState = EUIWidgetQueryMatchState::Parent;
			}
		}
	}
	
	if (!this->Name.IsEmpty())
	{
		FString WidgetName = Widget->GetName();

		// if not matches
		if ((!this->bIsNameAsKeyword && !WidgetName.Equals(this->Name)) ||
			(this->bIsNameAsKeyword && !WidgetName.Contains(this->Name)))
		{
			if (!bIsKeptParentClass || MatchState != EUIWidgetQueryMatchState::All)
			{
				// impossible to become a parent
				return EUIWidgetQueryMatchState::None;
			}
			else
			{
				// All/Parent -> Parent
				MatchState = EUIWidgetQueryMatchState::Parent;
			}
		}
	}

	if (!this->Text.IsEmpty())
	{
		const FString* WidgetText = FWidgetService::GetWidgetText(Widget);

		// if not matches
		if ((WidgetText == nullptr) || 
			(!this->bIsTextAsKeyword && !WidgetText->Equals(this->Text)) ||
			(this->bIsTextAsKeyword && !WidgetText->Contains(this->Text)))
		{
			if (!bIsKeptParentClass || MatchState != EUIWidgetQueryMatchState::All)
			{
				// impossible to become a parent
				return EUIWidgetQueryMatchState::None;
			}
			else
			{
				// All/Parent -> Parent
				MatchState = EUIWidgetQueryMatchState::Parent;
			}
		}
	}

	return MatchState;
}
