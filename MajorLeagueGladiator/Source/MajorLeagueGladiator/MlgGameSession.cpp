// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameSession.h"
#include "MlgPlayerController.h"

#include "OnlineSubsystemSessionSettings.h"


AMlgGameSession::AMlgGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMlgGameSession::OnCreateSessionComplete);

		OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &AMlgGameSession::OnFindSessionsComplete);
		OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMlgGameSession::OnJoinSessionComplete);

		OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &AMlgGameSession::OnStartOnlineGameComplete);
	}
}



/** Handle starting the match */
void AMlgGameSession::HandleMatchHasStarted()
{
	// start online game locally and wait for completion
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	check(Sessions.IsValid());
	
	OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	Sessions->StartSession(GameSessionName);	
}

void AMlgGameSession::OnStartOnlineGameComplete(FName InSessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	check(Sessions.IsValid())
	Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

	if (bWasSuccessful)
	{
		// tell non-local players to start online game
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AMlgPlayerController* PC = Cast<AMlgPlayerController>(*It);
			if (PC && !PC->IsLocalPlayerController())
			{
				PC->ClientStartOnlineGame();
			}
		}
	}
}

void AMlgGameSession::HandleMatchHasEnded()
{
	// start online game locally and wait for completion
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	check(Sessions.IsValid())
	
	// tell the clients to end
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMlgPlayerController* PC = Cast<AMlgPlayerController>(*It);
		if (PC && !PC->IsLocalPlayerController())
		{
			PC->ClientEndOnlineGame();
		}
	}

	Sessions->EndSession(GameSessionName);
}

bool AMlgGameSession::IsBusy() const
{
	if (HostSettings.IsValid() || SearchSettings.IsValid())
	{
		return true;
	}

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check (OnlineSub)
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	check(Sessions.IsValid());
	
	EOnlineSessionState::Type GameSessionState = Sessions->GetSessionState(GameSessionName);
	EOnlineSessionState::Type PartySessionState = Sessions->GetSessionState(PartySessionName);
	return GameSessionState != EOnlineSessionState::NoSession || PartySessionState != EOnlineSessionState::NoSession;
}

const TArray<FOnlineSessionSearchResult>& AMlgGameSession::GetSearchResults() const
{
	return SearchSettings->SearchResults;
};


bool AMlgGameSession::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	CurrentSessionParams.SessionName = InSessionName;
	CurrentSessionParams.bIsLAN = bIsLan;
	CurrentSessionParams.bIsPresence = bIsPresence;
	CurrentSessionParams.UserId = UserId;
	MaxPlayers = MaxNumPlayers;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
	{
		//HostSettings = MakeShareable(new FOnlineSessionSettings(bIsLAN, bIsPresence, MaxPlayers));
		HostSettings = MakeShareable(new FOnlineSessionSettings());
		HostSettings->bIsLANMatch = bIsLan;
		HostSettings->bUsesPresence = bIsPresence;
		HostSettings->NumPublicConnections = MaxNumPlayers;
		HostSettings->NumPrivateConnections = 0;
		HostSettings->bAllowInvites = true;
		HostSettings->bAllowJoinInProgress = true;
		HostSettings->bShouldAdvertise = true;
		HostSettings->bAllowJoinViaPresence = true;
		HostSettings->bAllowJoinViaPresenceFriendsOnly = false;

		OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		return Sessions->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
	}
	
	UE_LOG(DebugLog, Warning, TEXT("AMlgGameSession::HostSession failed"));

	return false;
}

void AMlgGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub)
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);	

	CreateSessionCompleteEvent.Broadcast(InSessionName, bWasSuccessful);
}



void AMlgGameSession::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	CurrentSessionParams.SessionName = InSessionName;
	CurrentSessionParams.bIsLAN = bIsLAN;
	CurrentSessionParams.bIsPresence = bIsPresence;
	CurrentSessionParams.UserId = UserId;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
	{
		SearchSettings = MakeShareable(new FOnlineSessionSearch());
		SearchSettings->bIsLanQuery = bIsLAN;

		TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();

		OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
		Sessions->FindSessions(*CurrentSessionParams.UserId, SearchSettingsRef);
	}	
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("AMlgGameSession::FindSessions failed"));
	}
	
}

void AMlgGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	check(Sessions.IsValid());
	
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	for (int32 SearchIdx = 0; SearchIdx < SearchSettings->SearchResults.Num(); SearchIdx++)
	{
		const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
		DumpSession(&SearchResult.Session);
	}

	FindSessionsCompleteEvent.Broadcast(bWasSuccessful);
}

bool AMlgGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, int32 SessionIndexInSearchResults)
{
	if (SessionIndexInSearchResults >= 0 && SessionIndexInSearchResults < SearchSettings->SearchResults.Num())
	{
		return JoinSession(UserId, InSessionName, SearchSettings->SearchResults[SessionIndexInSearchResults]);
	}
	return false;
}

bool AMlgGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (Sessions.IsValid() && UserId.IsValid())
	{
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		return Sessions->JoinSession(*UserId, InSessionName, SearchResult);
	}	

	return false;
}

void AMlgGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	
	JoinSessionCompleteEvent.Broadcast(Result);
}