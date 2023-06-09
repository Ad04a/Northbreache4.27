// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Headers/UIDelegates.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/EngineTypes.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabMultiplayerDataModels.h"

#include "NorthbreachGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayFabErrorSignature, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyStateSignature, bool, State);

UCLASS()
class NORTHBREACH_API UAccountInfo : public UObject
{
	GENERATED_BODY()

public:
	
	PlayFab::ClientModels::FLoginResult LoginInfo;

	FTimerHandle MatchmakingHandle;
	
	FString CurrentQueueName;
	FString CurrentTicketID;
};

UCLASS()
class NORTHBREACH_API UNorthbreachGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly)
	FString TitleID = "D30D1";

	UPROPERTY()
	UAccountInfo* AccountInfo = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	FName LobbyLevel = "Gameplay";

	UPROPERTY(EditDefaultsOnly)
	FString PathToArena;

	UPROPERTY(EditDefaultsOnly)
	bool bIsDeveloper = true;

	UPROPERTY(EditDefaultsOnly)
	FString DevLoopback = "127.0.0.1:8081";

	UPROPERTY(EditDefaultsOnly)
	FString CredentialName = "Northbreach";

	bool bIsLobbyOpen = false;

	PlayFabClientPtr ClientAPI = nullptr;
	PlayFabMultiplayerPtr MultiplayerAPI = nullptr;

	UFUNCTION()
	void OnLoginSuccess();

	void OnPlayfabLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result) const;

	void OnCreateTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result) const;
	void OnGetTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result) const;
	void OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result) const;

	void OnFail(const PlayFab::FPlayFabCppError& ErrorResult) const;

	void GetToLobby() const;
	void CheckIfMatchFound() const;

public:

	FPlayFabErrorSignature OnPlayFabError;
	FButtonClickedSignature OnSuccessfulRegister;
	FButtonClickedSignature OnLevelOpneingStarted;
	FLobbyStateSignature OnLobbyStateChange;

	virtual void OnStart() override;

	UFUNCTION()
	void CreateMatchmakingTicket(FString QueueName = "Quickplay") const;

	UFUNCTION()
	void GetMatchmakingTicket(FString QueueName = "Quickplay", FString TicketID = "") const;

	UFUNCTION()
	void GetMatch(FString MatchID, FString QueueName = "Quickplay") const;

	bool CanProduceTicket() { return AccountInfo->CurrentTicketID == ""; }


	UFUNCTION()
	void OpenLobby();

	UFUNCTION()
	void StartMatchmaking();

	void UpdateLobbyStatus();

};
