// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinWidgetBase.h"

#include "Components/TextBlock.h"

void UJoinWidgetBase::RecieveErrorMessage(FString Error)
{
	ErrorMessage->SetText(FText::FromString(Error));
}