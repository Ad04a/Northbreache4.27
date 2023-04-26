// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidgetBase.h"

#include "Components/TextBlock.h"
#include "Components/Border.h"

void UInteractionWidgetBase::SetInteractions(FText Basic, FText Right, FText Left)
{

	BasicText->SetText(Basic);
	if (Basic.ToString() != "")
	{
		BasicBackGround->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BasicBackGround->SetVisibility(ESlateVisibility::Hidden);
	}

	RightText->SetText(Right);
	if (Right.ToString() != "")
	{
		RightBackGround->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		RightBackGround->SetVisibility(ESlateVisibility::Hidden);
	}

	LeftText->SetText(Left);
	if (Left.ToString() != "")
	{
		LeftBackGround->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		LeftBackGround->SetVisibility(ESlateVisibility::Hidden);
	}
}