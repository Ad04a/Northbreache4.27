// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayModeBase.h"

#include "Characters/NorthbreachCharacter.h"
#include "NorthbreachPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGameplayModeBase::AGameplayModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameplayModeBase::StartPlay() {

	Super::StartPlay();

	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayModeBase::StartPlay IsValid(GameInstance) == false"));
		return;
	}

	if (GameInstance->IsDedicatedServerInstance() == true)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayModeBase::StartPlay IsValid(World) == false"));
		return;
	}
	
	ANorthbreachPlayerController* PlayerController = Cast<ANorthbreachPlayerController>(UGameplayStatics::GetPlayerController(World, 0));

	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayModeBase::StartPlay IsValid(PlayerController) == false"));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	Player = World->SpawnActor<ANorthbreachCharacter>(PlayerClass, SpawnLocation, FRotator(0, 0, 0), Params);
	PlayerController->Possess(Player);

	

}
