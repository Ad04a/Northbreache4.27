// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ServerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoginSignatrue);

UCLASS()
class NORTHBREACH_API UServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	bool bIsInitialized = false;
	void Setup(bool InIsDeveloper, FString InDevLoopback, FString InCredentialName);

	FLoginSignatrue OnLogin;

	UFUNCTION()
	void CreateDedicatedServerSession(FString InMapPath, bool bIsDedicated=true, FName SessionName= "Northbreach");

	UFUNCTION()
	void OnSessionCreationReply(FName SessionName, bool bIsSuccessful);

	UFUNCTION()
	void DestroySession(FName SessionName);

	UFUNCTION()
	void OnSessionDestroyReply(FName SessionName, bool bIsSuccessful);

	void EOSLogin();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	void FindSessionAndJoin();

	void OnSessionFoundSuccess(bool bWasSuccess);

	//void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	class IOnlineSubsystem* OnlineSubsystem;

	//TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY()
	FString MapPath;

	UPROPERTY()
	bool bIsDeveloper = true;

	UPROPERTY()
	FString DevLoopback = "127.0.0.1:8081";

	UPROPERTY()
	FString CredentialName = "Northbreach";

	FString GetAuthToken();
};
