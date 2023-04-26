// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Gameplay/Menus/MatchMenuBase.h"

#include "Components/Button.h"

void UMatchMenuBase::NativeOnInitialized()
{
	StartMatch->OnClicked.AddDynamic(this, &UMatchMenuBase::StartMatchClicked);
	Close->OnClicked.AddDynamic(this, &UMatchMenuBase::CloseClicked);
}

void UMatchMenuBase::StartMatchClicked()
{
	OnStartMatchClicked.Broadcast();
}

void UMatchMenuBase::CloseClicked()
{
	OnCloseClicked.Broadcast(this);
}