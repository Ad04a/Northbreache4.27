// Fill out your copyright notice in the Description page of Project Settings.

#include "NorthbreachPlayerController.h"

#include "Headers/Interactable.h"
#include "Characters/NorthbreachCharacter.h"
#include "UI/HUDS/GameplayHUD.h"

const FName ANorthbreachPlayerController::MoveHorizontalBinding("MoveHorizontal");
const FName ANorthbreachPlayerController::MoveVerticalBinding("MoveVertical");

const FName ANorthbreachPlayerController::BasicInteractBinding("BasicInteraction");
const FName ANorthbreachPlayerController::RightInteractBinding("RightInteraction");
const FName ANorthbreachPlayerController::LeftInteractBinding("LeftInteraction");

const FName ANorthbreachPlayerController::RightHandBinding("RightHand");
const FName ANorthbreachPlayerController::LeftHandBinding("LeftHand");
const FName ANorthbreachPlayerController::RightHandHeavyBinding("RightHandHeavy");
const FName ANorthbreachPlayerController::LeftHandHeavyBinding("LeftHandHeavy");
const FName ANorthbreachPlayerController::RightHandDropBinding("RightHandDrop");
const FName ANorthbreachPlayerController::LeftHandDropBinding("LeftHandDrop");

void ANorthbreachPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	if (IsValid(InputComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ANorthbreachPlayerController::OnPossess IsValid(InputComponent) == false"));
		return;
	}

	Fighter = Cast<ANorthbreachCharacter>(InPawn);
	if (IsValid(Fighter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ANorthbreachPlayerController::OnPossess IsValid(Player) == false"));
		return;
	}

	AGameplayHUD* HUD = Cast<AGameplayHUD>(GetHUD());
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ANorthbreachPlayerController::OnPossess IsValid(HUD) == false"));
		return;
	}

	InputComponent->BindAxis(MoveVerticalBinding, Fighter, &ANorthbreachCharacter::MoveVertical);
	InputComponent->BindAxis(MoveHorizontalBinding, Fighter, &ANorthbreachCharacter::MoveHorizontal);

	InputComponent->BindAction(BasicInteractBinding, IE_Pressed, this, &ANorthbreachPlayerController::BasicInteract);
	InputComponent->BindAction(RightInteractBinding, IE_Pressed, this, &ANorthbreachPlayerController::RightInteract);
	InputComponent->BindAction(LeftInteractBinding, IE_Pressed, this, &ANorthbreachPlayerController::LeftInteract);

	Fighter->OnInteractionChanges.BindDynamic(HUD, &AGameplayHUD::ToggleInteractionWidget);
}

void ANorthbreachPlayerController::BasicInteract()
{
	Fighter->Interact(EInteractionType::Basic);
}

void ANorthbreachPlayerController::RightInteract()
{
	Fighter->Interact(EInteractionType::Right);
}

void ANorthbreachPlayerController::LeftInteract()
{
	Fighter->Interact(EInteractionType::Left);
}