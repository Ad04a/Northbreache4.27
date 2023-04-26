// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "NorthbreachCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANorthbreachCharacter::ANorthbreachCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->SetRelativeRotation_Direct(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ANorthbreachCharacter::OnOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ANorthbreachCharacter::OnEndOverlap);

}


void ANorthbreachCharacter::MoveHorizontal(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void ANorthbreachCharacter::MoveVertical(float Value)
{
	// add movement in that direction
	if (Value > 0)
	{
		Jump();
	}
}

void ANorthbreachCharacter::Interact(EInteractionType InteractionType)
{
	if (InteractableActors.Num() < 1)
	{
		return;
	}

	IInteractable* InteractableActor = Cast<IInteractable>(InteractableActors[0]);
	InteractableActor->Execute_InteractRequest(InteractableActors[0], this, InteractionType);
}

void ANorthbreachCharacter::ManageInteractables(AActor* ActorToManage)
{
	if (InteractableActors.Contains(ActorToManage) == false)
	{
		InteractableActors.Add(ActorToManage);
	}
	else
	{
		InteractableActors.Remove(ActorToManage);
	}
	if (InteractableActors.Num() < 1)
	{
		UpdateInteractWidget(false);
		return;
	}
	UpdateInteractWidget(true);
}

void ANorthbreachCharacter::UpdateInteractWidget(bool bIsShowing)
{

	TArray<FInteractionText> InteractionMessages;

	if (InteractableActors.Num() < 1)
	{
		OnInteractionChanges.ExecuteIfBound(bIsShowing, InteractionMessages);
		return;
	}

	IInteractable* InteractableActor = Cast<IInteractable>(InteractableActors[0]);
	TMap<TEnumAsByte<EInteractionType>, FText> InteractionTexts = InteractableActor->Execute_DisplayInteractText(InteractableActors[0]);

	for (TPair<TEnumAsByte<EInteractionType>, FText> Text : InteractionTexts)
	{
		FInteractionText InteractionMessage;
		InteractionMessage.Type		= Text.Key;
		InteractionMessage.Message	= Text.Value;
		InteractionMessages.Add(InteractionMessage);
	}

	OnInteractionChanges.ExecuteIfBound(bIsShowing, InteractionMessages);
}

void ANorthbreachCharacter::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractable* InteractableActor = Cast<IInteractable>(OtherActor);
	if (InteractableActor == nullptr)
	{
		return;
	}

	ManageInteractables(OtherActor);

}

void ANorthbreachCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractable* InteractableActor = Cast<IInteractable>(OtherActor);
	if (InteractableActor == nullptr)
	{
		return;
	}

	ManageInteractables(OtherActor);
}
