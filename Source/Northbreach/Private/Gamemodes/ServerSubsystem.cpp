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

void UServerSubsystem::Setup(bool InIsDeveloper, FString InDevLoopback, FString InCredentialName)
{
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
	
	/*FOnlineSessionSetting compoundSessionName;
	compoundSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	compoundSessionName.Data = FString("Northbreach");*/
	//SessionCreationSettings.Settings.Add(FName("SESSION_NAME"), SessionName);

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

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionCreationReply IsValid(World) == false"));
		return;
	}
	World->ServerTravel(MapPath);
	
}


void UServerSubsystem::FindSessionAndJoin()
{
	/*IOnlineSessionPtr SessionPtr = Online::GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin SessionPtr == false"));
		return;
	}
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	//SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, SessionIdentifier.ToString(), EOnlineComparisonOp::Equals);
	FOnlineSessionSetting compoundSessionName;
	compoundSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	compoundSessionName.Data = FString("Northbreach");
	SessionSearch->QuerySettings.SearchParams.Add(FName("SESSION_NAME"), FOnlineSessionSearchParam(SessionIdentifier.ToString(), EOnlineComparisonOp::Equals));
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1000;
	UE_LOG(LogTemp, Display, TEXT("Searhing session"));
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerSubsystem::OnSessionFoundSuccess);
	SessionPtr->FindSessions(0, SessionSearch.ToSharedRef());*/
}

void UServerSubsystem::OnSessionFoundSuccess(bool bWasSuccess)
{
	/*if (bWasSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnSessionFoundSuccess Cannot join session"));
		return;
	}
	IOnlineSessionPtr SessionPtr = Online::GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin SessionPtr == false"));
		return;
	}
	if (SessionSearch.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin SessionSearch.IsValid() == false"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("%d sessions found"), SessionSearch->SearchResults.Num());
	if (SessionSearch->SearchResults.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Canceling operation"));
		return;
	}
	
	if (SessionSearch->SearchResults[0].IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin SessionSearch->SearchResult[0].IsValid() == false"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Trying to join session"));
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnJoinSessionComplete);
	SessionPtr->JoinSession(0, SessionIdentifier, SessionSearch->SearchResults[0]);*/
}

/*void UServerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::OnJoinSessionComplete Cannot join session"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Joining session"));
	FString JoinAdress;
	IOnlineSessionPtr SessionPtr = Online::GetSessionInterface();
	if (SessionPtr.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UServerSubsystem::FindSessionAndJoin SessionPtr == false"));
		return;
	}
	SessionPtr->GetResolvedConnectString(FName("Northbreach"), JoinAdress);
	
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
	PlayerController->ClientTravel(JoinAdress, ETravelType::TRAVEL_Absolute);
}*/