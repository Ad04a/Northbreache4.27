// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/Interactable.h"
#include "Headers/UIDelegates.h"
#include "MatchBoard.generated.h"

UCLASS()
class NORTHBREACH_API AMatchBoard : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* InteractRadius;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	FText InteractionText;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EMenuTypes> WidgetMenuType;

	virtual void InteractRequest_Implementation(class ANorthbreachCharacter* InteractingCharacter, EInteractionType InteractionType);

	virtual TMap<TEnumAsByte<EInteractionType>, FText> DisplayInteractText_Implementation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
