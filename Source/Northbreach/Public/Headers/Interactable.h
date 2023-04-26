// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Containers/Map.h"
#include "Interactable.generated.h"

UENUM(BlueprintType)
enum EInteractionType
{
	Basic	UMETA(DisplayName = "Basic"),
	Right	UMETA(DisplayName = "Right"),
	Left	UMETA(DisplayName = "Left")
};

USTRUCT()
struct FInteractionText
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EInteractionType> Type;

	UPROPERTY()
	FText Message;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FInteractVisualizationSignature, bool, bIsShowing, TArray<FInteractionText>, InteractionTexts);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NORTHBREACH_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void InteractRequest(class ANorthbreachCharacter* InteractingCharacter, EInteractionType InteractionType);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	TMap<TEnumAsByte<EInteractionType>, FText> DisplayInteractText();
};
