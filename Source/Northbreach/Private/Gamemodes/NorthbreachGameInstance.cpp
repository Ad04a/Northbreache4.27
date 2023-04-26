// Fill out your copyright notice in the Description page of Project Settings.


#include "NorthbreachGameInstance.h"

#include "ServerSubsystem.h"

#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerApi.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UNorthbreachGameInstance::OnStart()
{
	Super::OnStart();

	UServerSubsystem* ServerSubsystem = GetSubsystem<UServerSubsystem>();

	ServerSubsystem->Setup(bIsDeveloper, DevLoopback, CredentialName);

	if (IsDedicatedServerInstance() == true)
	{
		ServerSubsystem->CreateDedicatedServerSession(PathToArena);

		return;
	}
	ServerSubsystem->OnLogin.AddDynamic(this, &UNorthbreachGameInstance::OnLoginSuccess);
	ServerSubsystem->EOSLogin();
	ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	MultiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();
	AccountInfo = NewObject<UAccountInfo>(this);

	
}

//-------------------------------------------------LOGIN-----------------------------------------------------------------------------------

void UNorthbreachGameInstance::OnLoginSuccess()
{
	OnLevelOpneingStarted.Broadcast();

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnLoginSuccess IsValid(World) == false"));
		return;
	}

	FTimerHandle UnusedHandle;
	World->GetTimerManager().SetTimer(
		UnusedHandle, this, &UNorthbreachGameInstance::GetToLobby, 2, false);
}

void UNorthbreachGameInstance::GetToLobby()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnLoginSuccess IsValid(World) == false"));
		return;
	}
	UGameplayStatics::OpenLevel(World, LobbyLevel);
}

void UNorthbreachGameInstance::OpenLobby()
{
	UServerSubsystem* ServerSubsystem = GetSubsystem<UServerSubsystem>();

	ServerSubsystem->CreateDedicatedServerSession(FString(), false, FName("Northbreach_Lobby"));
}

void UNorthbreachGameInstance::StartMatchmaking()
{

}


















//-------------------------------------------------REGISTER-----------------------------------------------------------------------------------

void UNorthbreachGameInstance::Register(FString Username, FString Email, FString Password)
{
	PlayFab::ClientModels::FRegisterPlayFabUserRequest Request;
	Request.Email = Email;
	Request.Username = Username;
	Request.DisplayName = Username;
	Request.Password = Password;
	Request.TitleId = TitleID;

	ClientAPI->RegisterPlayFabUser(Request,
		PlayFab::UPlayFabClientAPI::FRegisterPlayFabUserDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnRegisterSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnFail));
}


void UNorthbreachGameInstance::OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result) const
{
	OnSuccessfulRegister.Broadcast();
}

//-------------------------------------------------PASSWORD-RESET-----------------------------------------------------------------------------------

void UNorthbreachGameInstance::Reset(FString Email)
{
	PlayFab::ClientModels::FSendAccountRecoveryEmailRequest Request;
	Request.Email = Email;
	Request.TitleId = TitleID;

	ClientAPI->SendAccountRecoveryEmail(Request,
		PlayFab::UPlayFabClientAPI::FSendAccountRecoveryEmailDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnResetSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnFail));
}

void UNorthbreachGameInstance::OnResetSuccess(const PlayFab::ClientModels::FSendAccountRecoveryEmailResult& Result) const
{ 
	OnPlayFabError.Broadcast("Recovery Email has been sent successfully");
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------MATCHMAKING-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------CREAT-TICKET-----------------------------------------------------------------------------------

void UNorthbreachGameInstance::CreateMatchmakingTicket(FString QueueName) const
{
	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Creating matchmaking ticket for queue{ %s }"), *QueueName);
	AccountInfo->CurrentQueueName = QueueName;

	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest Request;

	Request.GiveUpAfterSeconds = 120;
	Request.QueueName = QueueName;
	Request.Creator = PlayFab::MultiplayerModels::FMatchmakingPlayer();
	Request.Creator.Entity.Id = AccountInfo->LoginInfo.EntityToken->Entity->Id;
	Request.Creator.Entity.Type = AccountInfo->LoginInfo.EntityToken->Entity->Type;

	MultiplayerAPI->CreateMatchmakingTicket(Request,
		PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnCreateTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnFail));
}

void UNorthbreachGameInstance::OnCreateTicketSuccess(const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result) const
{
	AccountInfo->CurrentTicketID = Result.TicketId;
	
	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Created ticked[%s] for queue{ %s }"), *AccountInfo->CurrentTicketID, *AccountInfo->CurrentQueueName);

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnLoginSuccess IsValid(World) == false"));
		return;
	}
	CheckIfMatchFound();
	World->GetTimerManager().SetTimer(
		AccountInfo->MatchmakingHandle, this, &UNorthbreachGameInstance::CheckIfMatchFound, 10, true);
}

void UNorthbreachGameInstance::CheckIfMatchFound() const
{
	GetMatchmakingTicket(AccountInfo->CurrentQueueName, AccountInfo->CurrentTicketID);
}

//---------------------------------------------------GET-TIKCET-----------------------------------------------------------------------------------

void UNorthbreachGameInstance::GetMatchmakingTicket(FString QueueName, FString TicketID) const
{
	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest Request;

	Request.QueueName = QueueName;
	Request.TicketId  = TicketID;

	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Ticked[%s] for queue{ %s } is searching"), *AccountInfo->CurrentTicketID, *AccountInfo->CurrentQueueName);

	MultiplayerAPI->GetMatchmakingTicket(Request,
		PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnGetTicketSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnFail));

}

void UNorthbreachGameInstance::OnGetTicketSuccess(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& Result) const
{
	if (Result.Status != "Matched")
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Ticked[%s] for queue{ %s } found Match[%s]"), *AccountInfo->CurrentTicketID, *AccountInfo->CurrentQueueName, *Result.MatchId);

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnLoginSuccess IsValid(World) == false"));
		return;
	}

	World->GetTimerManager().ClearTimer(AccountInfo->MatchmakingHandle);
	
	AccountInfo->CurrentQueueName = "";
	AccountInfo->CurrentTicketID  = "";

	GetMatch(Result.MatchId, Result.QueueName);

}

//---------------------------------------------------GET-MATCH------------------------------------------------------------------------------------

void UNorthbreachGameInstance::GetMatch(FString MatchID, FString QueueName) const
{
	PlayFab::MultiplayerModels::FGetMatchRequest Request;

	Request.MatchId = MatchID;
	Request.QueueName = QueueName;

	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Ticked[%s] for queue{ %s } entering Match[%s]"), *AccountInfo->CurrentTicketID, *AccountInfo->CurrentQueueName, *MatchID);

	MultiplayerAPI->GetMatch(Request,
		PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnGetMatchSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNorthbreachGameInstance::OnFail));
}

void UNorthbreachGameInstance::OnGetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& Result) const
{
	//FString JoinCommand = "open ";
	//if (Result.pfServerDetails == nullptr)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnGetMatchSuccess() Server Details not found"));
	//	return;
	//}
	//JoinCommand.Append(Result.pfServerDetails->IPV4Address);
	//JoinCommand.Append(":");
	//JoinCommand.Append(FString::FromInt(Result.pfServerDetails->Ports[0].Num));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnGetMatchSuccess() IsValid(PlayerController) == false"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UNorthbreachGameInstance::OnLoginSuccess IsValid(World) == false"));
		return;
	}

	UServerSubsystem* ServerSubsystem = GetSubsystem<UServerSubsystem>();
	ServerSubsystem->FindSessionAndJoin();

	World->GetTimerManager().SetTimer(
		AccountInfo->MatchmakingHandle, ServerSubsystem, &UServerSubsystem::FindSessionAndJoin, 6, true);

	UE_LOG(LogTemp, Display, TEXT("UNorthbreachGameInstance : Player joined Match[%s]"), *Result.MatchId);
}

//---------------------------------------------------ERRORS---------------------------------------------------------------------------------------
void UNorthbreachGameInstance::OnFail(const PlayFab::FPlayFabCppError& ErrorResult) const
{
	TArray<FString> Keys;
	ErrorResult.ErrorDetails.GenerateKeyArray(Keys);
	FString LogError = ErrorResult.GenerateErrorReport();
	FString Error = ErrorResult.ErrorName;
	if( Keys.Num() <= 0)
	{
		OnPlayFabError.Broadcast(Error);
		return;
	}
	Error += " : ";
	for (FString Key : Keys)
	{
		Error += Key + ", ";
	}
	UE_LOG(LogTemp, Error, TEXT("PlayFab Request Faild : [%s]"), *LogError);
	OnPlayFabError.Broadcast(Error);
}