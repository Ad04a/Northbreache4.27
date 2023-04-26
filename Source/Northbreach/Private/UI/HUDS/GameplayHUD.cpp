// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayHUD.h"

#include "Gamemodes/NorthbreachGameInstance.h"

#include "UI/Widgets/Gameplay/InteractionWidgetBase.h"
#include "UI/Widgets/Gameplay/Menus/FriendsMenuBase.h"
#include "UI/Widgets/Gameplay/Menus/MatchMenuBase.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


void AGameplayHUD::BeginPlay()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayHUD::BeginPlay IsValid(World) == false"));
		return;
	}

	UNorthbreachGameInstance* GameInstance = Cast<UNorthbreachGameInstance>(UGameplayStatics::GetGameInstance(World));

	InteractionWidget = CreateWidget<UInteractionWidgetBase>(World, InteractionWidgetClass);
	if (IsValid(InteractionWidget) == false) {
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(LoadingScreenWidget) == false"));
		return;
	}

	FriendsMenuWidget = CreateWidget<UFriendsMenuBase>(World, FriendsMenuClass);
	if (IsValid(FriendsMenuWidget) == false) {
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(FriendsMenuWidget) == false"));
		return;
	}

	FriendsMenuWidget->OnOpenLobbyClicked.AddDynamic(GameInstance, &UNorthbreachGameInstance::OpenLobby);
	FriendsMenuWidget->OnCloseClicked.AddDynamic(this, &AGameplayHUD::HideWidget);
	GameInstance->OnLobbyStateChange.AddDynamic(FriendsMenuWidget, &UFriendsMenuBase::SetLobbyText);
	GameInstance->UpdateLobbyStatus();

	MatchMenuWidget = CreateWidget<UMatchMenuBase>(World, MatchMenuClass);
	if (IsValid(MatchMenuWidget) == false) {
		UE_LOG(LogTemp, Error, TEXT("AMenuHUDBase::BeginPlay() IsValid(MatchMenuWidget) == false"));
		return;
	}

	MatchMenuWidget->OnStartMatchClicked.AddDynamic(GameInstance, &UNorthbreachGameInstance::StartMatchmaking);
	MatchMenuWidget->OnCloseClicked.AddDynamic(this, &AGameplayHUD::HideWidget);

	MenuMap.Add(EMenuTypes::Matches, MatchMenuWidget);
	MenuMap.Add(EMenuTypes::Friends, FriendsMenuWidget);

	PlayerOwner->bShowMouseCursor = false;
	PlayerOwner->SetInputMode(FInputModeGameOnly());
}

void AGameplayHUD::ToggleInteractionWidget(bool bIsShowing, TArray<FInteractionText> InteractionTexts)
{
	if ((IsValid(PlayerOwner) && IsValid(InteractionWidget)) == false) {
		return;
	}
	if (bIsShowing == false)
	{
		InteractionWidget->RemoveFromViewport();
		return;
	}

	FText Basic = FText::FromString("");
	FText Right = FText::FromString("");
	FText Left = FText::FromString("");

	for (FInteractionText Text : InteractionTexts)
	{
		if (Text.Type == EInteractionType::Right)
		{
			Right = Text.Message;
		}
		else if (Text.Type == EInteractionType::Left)
		{
			Left = Text.Message;
		}
		else
		{
			Basic = Text.Message;
		}
	}

	InteractionWidget->SetInteractions(Basic, Right, Left);
	InteractionWidget->AddToViewport();

}


void AGameplayHUD::ShowMenu(EMenuTypes MenuType)
{
	if (IsValid(PlayerOwner) == false) {
		return;
	}

	UUserWidget* Widget = *MenuMap.Find(MenuType);
	if (IsValid(Widget) == false) {
		return;
	}
	Widget->AddToViewport();
	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeUIOnly());
}

void AGameplayHUD::HideWidget(UUserWidget* Widget)
{
	Widget->RemoveFromViewport();
	PlayerOwner->bShowMouseCursor = false;
	PlayerOwner->SetInputMode(FInputModeGameOnly());
}
