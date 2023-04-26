// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NorthbreachPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API ANorthbreachPlayerController : public APlayerController
{
	GENERATED_BODY()

private:


protected:

	class ANorthbreachCharacter* Fighter;

	virtual void OnPossess(APawn* InPawn) override;

	static const FName MoveHorizontalBinding;
	static const FName MoveVerticalBinding;
	static const FName MoveUpBinding;
	static const FName MoveDownBinding;

	static const FName BasicInteractBinding;
	static const FName RightInteractBinding;
	static const FName LeftInteractBinding;

	static const FName RightHandBinding;
	static const FName LeftHandBinding;
	static const FName RightHandHeavyBinding;
	static const FName LeftHandHeavyBinding;
	static const FName RightHandDropBinding;
	static const FName LeftHandDropBinding;

	UFUNCTION()
	void BasicInteract();

	UFUNCTION()
	void RightInteract();
	
	UFUNCTION()
	void LeftInteract();

public:

};
