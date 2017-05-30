// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameInstance.h"

namespace
{
	const FString PRE_BEGIN_MAP("/Game/PreGame?game=/Script/MajorLeagueGladiator.PreGameGameMode?listen");
	const FString MAIN_MENU_MAP("/Game/MainMenu?game=Class'/Script/MajorLeagueGladiator.MenuGameMode'");


	IOnlineSessionPtr findOnlineSession()
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		check(OnlineSub);

		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		check(Sessions.IsValid());
		return Sessions;
	}
}

UMlgGameInstance::UMlgGameInstance(const FObjectInitializer & ObjectInitializer)
{
	onCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onCreateSessionComplete);
	onStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onStartSessionComplete);
	onFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onFindSessionsComplete);
	onJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onJoinSessionComplete);
	onDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onDestroySessionComplete);
}

bool UMlgGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	sessionSettings = MakeShareable(new FOnlineSessionSettings());
	check(sessionSettings.IsValid())

	sessionSettings->bIsLANMatch = bIsLan;
	sessionSettings->bUsesPresence = bIsPresence;
	sessionSettings->NumPublicConnections = MaxNumPlayers;
	sessionSettings->NumPrivateConnections = 0;
	sessionSettings->bAllowInvites = true;
	sessionSettings->bAllowJoinInProgress = true;
	sessionSettings->bShouldAdvertise = true;
	sessionSettings->bAllowJoinViaPresence = true;
	sessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	/*
	Fill in all the Session Settings that we want to use.
	There are more with SessionSettings.Set(...);
	For example the Map or the GameMode/Type.
	*/
	//sessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);

	onCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(onCreateSessionCompleteDelegate);

	return Sessions->CreateSession(*UserId, GameSessionName, *sessionSettings);
}

void UMlgGameInstance::onCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(onCreateSessionCompleteDelegateHandle);
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onCreateSessionComplete: CreateSession failed"));
		return;
	}

	onStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(onStartSessionCompleteDelegate);
	Sessions->StartSession(SessionName);
}

void UMlgGameInstance::onStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnStartSessionCompleteDelegate_Handle(onStartSessionCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel(PRE_BEGIN_MAP);
	}
}

void UMlgGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence, int32 MaxSearchResults, int32 PingBucketSize)
{
	if (onFindSessionsCompleteDelegateHandle.IsValid())
	{
		UE_LOG(DebugLog, Warning, TEXT(" UMlgGameInstance::FindSessions: Already Looking for Session"));
		return;
	}
	check(UserId->IsValid());

	IOnlineSessionPtr Sessions = findOnlineSession();
	/*
	Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
	*/
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->bIsLanQuery = bIsLAN;
	sessionSearch->MaxSearchResults = MaxSearchResults;
	sessionSearch->PingBucketSize = PingBucketSize;

	// We only want to set this Query Setting if "bIsPresence" is true
	if (bIsPresence)
	{
		sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = sessionSearch.ToSharedRef();

	// Set the Delegate to the Delegate Handle of the FindSession function
	onFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(onFindSessionsCompleteDelegate);

	// Finally call the SessionInterface function. The Delegate gets called once this is finished
	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

void UMlgGameInstance::onFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(onFindSessionsCompleteDelegateHandle);

	OnFindSessionFinished.Broadcast(sessionSearch->SearchResults);
}

bool UMlgGameInstance::JoinSession(ULocalPlayer* localPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	check(localPlayer->IsValidLowLevel());

	IOnlineSessionPtr Sessions = findOnlineSession();

	onJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(onJoinSessionCompleteDelegate);

	return Sessions->JoinSession(*localPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
}

void UMlgGameInstance::onJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(onJoinSessionCompleteDelegateHandle);

	// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
	// This is something the Blueprint Node "Join Session" does automatically!
	APlayerController*  PlayerController = GetFirstLocalPlayerController();

	// We need a FString to use ClientTravel and we can let the SessionInterface construct such a
	// String for us by giving him the SessionName and an empty String. We want to do this, because
	// Every OnlineSubsystem uses different TravelURLs
	FString TravelURL;

	if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
	{
		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}


void UMlgGameInstance::TravelToMainMenu()
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	if (Sessions.IsValid())
	{
		Sessions->AddOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteDelegate);
			
		Sessions->DestroySession(GameSessionName);
	}
}

void UMlgGameInstance::onDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteDelegateHandle);

	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel(MAIN_MENU_MAP);
	}
}

EOnlineSessionState::Type UMlgGameInstance::GetGameSessionState() const
{
	IOnlineSessionPtr Sessions = findOnlineSession();
	return Sessions->GetSessionState(GameSessionName);
}
