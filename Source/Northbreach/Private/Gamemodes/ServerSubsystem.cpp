// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSubsystem.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::Initialize OnlineSubsystem == nullptr"));
		return;
	}
	bIsInitialized = true;
}

void UServerSubsystem::Setup(bool InIsCurrentUserDedicated, bool InIsDeveloper, FString InDevLoopback, FString InCredentialName)
{
	bIsCurrentUserDedicated = InIsCurrentUserDedicated;
	bIsDeveloper = InIsDeveloper;
	DevLoopback = InDevLoopback;
	CredentialName = InCredentialName;
}

void UServerSubsystem::EOSLogin()
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::EOSLogin OSS is not initialized"));
		return;
	}
	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (Identity.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::EOSLogin Identity == false"));
		return;
	}


	FOnlineAccountCredentials Credentials;

	if (bIsDeveloper == false)
	{
		Credentials.Id = FString();
		Credentials.Token = FString();
		Credentials.Type = FString("accountportal");
	}
	else
	{
		Credentials.Id = DevLoopback;
		Credentials.Token = CredentialName;
		Credentials.Type = FString("developer");
	}

	Identity->OnLoginCompleteDelegates->AddUObject(this, &UServerSubsystem::OnLoginComplete);

	Identity->Login(0, Credentials);

}

void UServerSubsystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Error, TEXT("LoggedIn = %d"), bWasSuccessful);
	if (bWasSuccessful == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not login:\n%s"), *Error);
		return;
	}
	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (Identity.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::EOSLogin Identity == false"));
		return;
	}
	OnLogin.Broadcast();
	Identity->ClearOnLoginCompleteDelegates(0, this);
	
}

FString UServerSubsystem::GetAuthToken()
{
	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (Identity.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::GetAuthToken Identity == false"));
		return FString();
	}
	return Identity->GetAuthToken(0);
}

void UServerSubsystem::CreateDedicatedServerSession(FString InMapPath, bool bIsDedicated, FName SessionName)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::CreateDedicatedServerSession OSS is not initialized"));
		return;
	}
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::CreateDedicatedServerSession SessionPtr == false"));
		return;
	}

	if (bIsDedicated == true)
	{
		UE_LOG(LogTemp, Display, TEXT("Starting session for server"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Starting session for client"));
	}

	MapPath = InMapPath;

	FOnlineSessionSettings SessionCreationSettings;
	SessionCreationSettings.bShouldAdvertise = true;
	SessionCreationSettings.bIsLANMatch = false;
	SessionCreationSettings.NumPublicConnections = 2;
	SessionCreationSettings.bAllowJoinInProgress = true;
	SessionCreationSettings.bAllowInvites = false;
	SessionCreationSettings.bAllowJoinViaPresenceFriendsOnly = false;

	SessionCreationSettings.bIsDedicated = bIsDedicated;
	SessionCreationSettings.bUsesPresence = !bIsDedicated;
	SessionCreationSettings.bAllowJoinViaPresence = !bIsDedicated;
	SessionCreationSettings.bUseLobbiesIfAvailable = !bIsDedicated;

	SessionCreationSettings.Set(SEARCH_KEYWORDS, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnSessionCreationReply);
	SessionPtr->CreateSession(0, SessionName, SessionCreationSettings);
}


void UServerSubsystem::OnSessionCreationReply(FName SessionName, bool bIsSuccessful)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionCreationReply OSS is not initialized"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Session %s reply = %d"), *SessionName.ToString(), bIsSuccessful);
	if (bIsSuccessful == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Session creation faild"));
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::CreateDedicatedServerSession SessionPtr == false"));
		return;
	}
	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);

	if (bIsCurrentUserDedicated == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionCreationReply IsValid(World) == false"));
		return;
	}

	UGameplayStatics::OpenLevel(World, "Arena");

	World->ServerTravel(MapPath);

}

void UServerSubsystem::DestroySession(FName SessionName)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::DestroySession OSS is not initialized"));
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::DestroySession SessionPtr == false"));
		return;
	}
	SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnSessionDestroyReply);
	SessionPtr->DestroySession(SessionName);
}

void UServerSubsystem::OnSessionDestroyReply(FName SessionName, bool bIsSuccessful)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionDestroyReply OSS is not initialized"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Destoy Session %s reply = %d"), *SessionName.ToString(), bIsSuccessful);
	if (bIsSuccessful == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Session destroy faild"));
		return;
	}
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::CreateDedicatedServerSession SessionPtr == false"));
		return;
	}
	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
}


void UServerSubsystem::FindSessions(FName SessionName)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessions OSS is not initialized"));
		return;
	}

	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessions SessionPtr == false"));
		return;
	}

	CurrentSesssionSearch = SessionName;

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, SessionName.ToString(), EOnlineComparisonOp::Equals);
	SearchSettings->MaxSearchResults = 10000;
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerSubsystem::OnSessionFoundSuccess);

	SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());

}

void UServerSubsystem::OnSessionFoundSuccess(bool bWasSuccess)
{
	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionFoundSuccess OSS is not initialized"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Find Session reply = %d"), SearchSettings->SearchResults.Num());
	if (bWasSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Session search faild"));
		return;
	}
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessions SessionPtr == false"));
		return;
	}

	SessionPtr->ClearOnFindSessionsCompleteDelegates(this);

	if (SearchSettings->SearchResults.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessions No sessions found"));
		return;
	}
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnJoinSessionComplete);
	SessionPtr->JoinSession(0, CurrentSesssionSearch, SearchSettings->SearchResults[0]);
}

void UServerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnJoinSessionComplete Cannot join session"));
		return;
	}

	if (bIsInitialized == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionFoundSuccess OSS is not initialized"));
		return;
	}
	IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessions SessionPtr == false"));
		return;
	}
	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);
	UE_LOG(LogTemp, Display, TEXT("Joining session"));

	FString JoinAdress = FString();
	SessionPtr->GetResolvedConnectString(SessionName, JoinAdress);
	
	if (JoinAdress.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Display, TEXT(" Cannot resolve JoinAdress"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Joining adress: %s"), *JoinAdress);
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin IsValid(World) == false"));
		return;
	}
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin IsValid(PlayerController) == false"));
		return;
	}
	PlayerController->ClientTravel(JoinAdress, ETravelType::TRAVEL_Absolute);
}