// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Gameplay/Menus/FriendsMenuBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UFriendsMenuBase::NativeOnInitialized()
{
	OpenLobby->OnClicked.AddDynamic(this, &UFriendsMenuBase::OpenLobbyClicked);
	Close->OnClicked.AddDynamic(this, &UFriendsMenuBase::CloseClicked);
}

void UFriendsMenuBase::OpenLobbyClicked()
{
	OnOpenLobbyClicked.Broadcast();
}

void UFriendsMenuBase::CloseClicked()
{
	OnCloseClicked.Broadcast(this);
}

void UFriendsMenuBase::SetLobbyText(bool State)
{
	if (State == false)
	{
		LobbyBtnText->SetText(TextToOpen);
		return;
	}
	LobbyBtnText->SetText(TextToClose);
}