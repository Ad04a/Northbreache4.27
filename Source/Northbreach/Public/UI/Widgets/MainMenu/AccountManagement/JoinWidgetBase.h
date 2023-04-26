// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinWidgetBase.generated.h"

UCLASS()
class NORTHBREACH_API UJoinWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ErrorMessage;

public:

	UFUNCTION()
	void RecieveErrorMessage(FString Error);
};
