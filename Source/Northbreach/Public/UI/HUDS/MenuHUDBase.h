// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class NORTHBREACH_API AMenuHUDBase : public AHUD
{
	GENERATED_BODY()
	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJoinWidgetBase> JoinWidgetClass;

	UPROPERTY()
	class UJoinWidgetBase* JoinWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> LoadingScreenWidgetClass;

	UPROPERTY()
	class UUserWidget* LoadingScreenWidget;

	void Clear();

	void ShowJoin();

	UFUNCTION()
	void ShowLoadingScreen();

public:

	virtual void BeginPlay() override;
};
