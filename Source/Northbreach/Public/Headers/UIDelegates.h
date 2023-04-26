#pragma once

#include "UIDelegates.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonClickedSignature);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetPassingSignature, class UUserWidget*, Widget);

UENUM(BlueprintType)
enum EMenuTypes
{
	Friends	UMETA(DisplayName = "Friends"),
	Matches	UMETA(DisplayName = "Matches")
};