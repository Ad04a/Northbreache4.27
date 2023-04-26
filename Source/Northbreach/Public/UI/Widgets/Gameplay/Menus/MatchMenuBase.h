// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/UIDelegates.h"
#include "MatchMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API UMatchMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* StartMatch;

	UFUNCTION()
	void StartMatchClicked();


	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* Close;

	UFUNCTION()
	void CloseClicked();

public:


	virtual void NativeOnInitialized() override;

	FButtonClickedSignature OnStartMatchClicked;
	FWidgetPassingSignature OnCloseClicked;

};
