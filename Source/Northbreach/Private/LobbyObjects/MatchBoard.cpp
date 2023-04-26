// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchBoard.h"

#include "Gamemodes/NorthbreachGameInstance.h"
#include "Characters/NorthbreachCharacter.h"
#include "UI/HUDS/GameplayHUD.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMatchBoard::AMatchBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	InteractRadius	= CreateDefaultSubobject<UBoxComponent>(TEXT("InteractRadius"));
	InteractRadius->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AMatchBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMatchBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMatchBoard::InteractRequest_Implementation(class ANorthbreachCharacter* InteractingCharacter, EInteractionType InteractionType)
{
	if (InteractionType != EInteractionType::Basic)
	{
		return;
	}

	APlayerController* Controller = InteractingCharacter->GetController<APlayerController>();
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AMatchBoard::InteractRequest_Implementation IsValid(Controller) == false"));
		return;
	}
	AGameplayHUD* HUD = Cast<AGameplayHUD>(Controller->GetHUD());
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AMatchBoard::InteractRequest_Implementation IsValid(HUD) == false"));
		return;
	}

	HUD->ShowMenu(WidgetMenuType);
		
}

TMap<TEnumAsByte<EInteractionType>, FText> AMatchBoard::DisplayInteractText_Implementation()
{
	TMap<TEnumAsByte<EInteractionType>, FText> Interactions;
	Interactions.Add(EInteractionType::Basic, InteractionText);

	return Interactions;
}

