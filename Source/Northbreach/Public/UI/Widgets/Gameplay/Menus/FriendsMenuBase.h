// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/UIDelegates.h"
#include "FriendsMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API UFriendsMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* OpenLobby;

	UFUNCTION()
	void OpenLobbyClicked();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* Close;

	UFUNCTION()
	void CloseClicked();

public:

	virtual void NativeOnInitialized() override;

	FButtonClickedSignature OnOpenLobbyClicked;
	FWidgetPassingSignature OnCloseClicked;
};
