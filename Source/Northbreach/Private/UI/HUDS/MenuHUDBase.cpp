// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuHUDBase.h"

#include "UI/Widgets/MainMenu/AccountManagement/JoinWidgetBase.h"
#include "Gamemodes/NorthbreachGameInstance.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AMenuHUDBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(World) == false"));
		return;
	}
	
	UNorthbreachGameInstance* GameInstance = Cast<UNorthbreachGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(GameInstance) == false"));
		return; 
	}

	LoadingScreenWidget = CreateWidget<UUserWidget>(World, LoadingScreenWidgetClass);
	if (IsValid(LoadingScreenWidget) == false) {
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(LoadingScreenWidget) == false"));
		return;
	}

	JoinWidget = CreateWidget<UJoinWidgetBase>(World, JoinWidgetClass);
	if (IsValid(JoinWidget) == false) {
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(JoinWidget) == false"));
		return;
	}


	GameInstance->OnPlayFabError.AddDynamic(JoinWidget, &UJoinWidgetBase::RecieveErrorMessage);

	GameInstance->OnLevelOpneingStarted.AddDynamic(this, &AMenuHUDBase::ShowLoadingScreen);

	ShowJoin();

	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeUIOnly());
}


void AMenuHUDBase::ShowJoin()
{
	if ((IsValid(PlayerOwner) && IsValid(JoinWidget)) == false) {
		return;
	}

	Clear();

	JoinWidget->AddToViewport();

}

void AMenuHUDBase::ShowLoadingScreen()
{
	if ((IsValid(PlayerOwner) && IsValid(LoadingScreenWidget)) == false) {
		return;
	}

	Clear();

	LoadingScreenWidget->AddToViewport();
}

void AMenuHUDBase::Clear()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);
}

