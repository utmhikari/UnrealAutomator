#include "Model/UIModel.h"
#include "Service/WidgetService.h"


bool FUIWidgetQuery::IsMatch(UWidget* Widget, bool bIsDisabledIncluded, bool bIsInvisibleIncluded)
{
	if (Widget == nullptr)
	{
		return false;
	}

	if (!bIsDisabledIncluded && !Widget->GetIsEnabled())
	{
		return false;
	}

	if (!bIsInvisibleIncluded && !Widget->IsVisible())
	{
		return false;
	}

	if (this->ID == 0 && this->ClassName.Len() == 0 && this->Name.Len() == 0 && this->Text.Len() == 0)
	{
		// empty query
		return false;
	}

	if (this->ID != 0)
	{
		if (this->ID != Widget->GetUniqueID())
		{
			return false;
		}
	}

	if (this->ClassName.Len() != 0)
	{
		if (!this->ClassName.Equals(Widget->GetClass()->GetName()))
		{
			return false;
		}
	}

	if (this->Name.Len() != 0)
	{
		FString WidgetName = Widget->GetName();
		if (!this->bIsNameAsKeyword)
		{
			if (!WidgetName.Equals(this->Name))
			{
				return false;
			}
		}
		else
		{
			if (!WidgetName.Contains(this->Name))
			{
				return false;
			}
		}
	}

	if (this->Text.Len() != 0)
	{
		const FString* WidgetText = FWidgetService::GetWidgetText(Widget);
		if (WidgetText == nullptr)
		{
			return false;
		}
		if (!this->bIsTextAsKeyword)
		{
			if (!WidgetText->Equals(this->Text))
			{
				return false;
			}
		}
		else
		{
			if (!WidgetText->Contains(this->Text))
			{
				return false;
			}
		}
	}

	return true;
}
