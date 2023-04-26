// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API UInteractionWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* BasicText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* RightText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* LeftText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UBorder* BasicBackGround;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UBorder* RightBackGround;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UBorder* LeftBackGround;

public:
	void SetInteractions(FText Basic, FText Right, FText Left);
};
