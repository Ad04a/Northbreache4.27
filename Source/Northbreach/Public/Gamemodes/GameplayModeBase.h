// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API AGameplayModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGameplayModeBase();

	virtual void StartPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly)
	FVector SpawnLocation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ANorthbreachCharacter> PlayerClass;

	class ANorthbreachCharacter* Player;

};
