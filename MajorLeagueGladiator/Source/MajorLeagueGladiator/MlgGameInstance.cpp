// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameInstance.h"

namespace
{
	const FString PRE_BEGIN_MAP("/Game/PreGame?game=Blueprint'/Game/BluePrints/MlgGameModeBP.MlgGameModeBP'");
	const FString MAIN_MENU_MAP("/Game/MainMenu?game=Class'/Script/MajorLeagueGladiator.MenuGameMode'");
	const FString DefaultFriendsList(EFriendsLists::ToString(EFriendsLists::Default));
	

	IOnlineSubsystem* getOnlineSubsystem()
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		check(OnlineSub);
		return OnlineSub;
	}

	IOnlineSessionPtr findOnlineSession()
	{
		IOnlineSessionPtr Sessions = getOnlineSubsystem()->GetSessionInterface();
		check(Sessions.IsValid());
		return Sessions;
	}

	IOnlineFriendsPtr findOnlineFriends()
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		check(OnlineSub);

		IOnlineFriendsPtr Sessions = getOnlineSubsystem()->GetFriendsInterface();
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

	IOnlineSessionPtr Sessions = findOnlineSession();
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
		bIsInRoomOfShame = true;
		GetWorld()->ServerTravel(PRE_BEGIN_MAP);
	}
	else
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onStartSessionComplete: StartSession failed"));
		return;
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

	IOnlineSessionPtr Sessions = findOnlineSession();
	onFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(onFindSessionsCompleteDelegate);

	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

void UMlgGameInstance::onFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(onFindSessionsCompleteDelegateHandle);

	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onFindSessionsComplete: findSession failed"));
		return;
	}

	OnFindSessionFinished.Broadcast(sessionSearch->SearchResults);
}

bool UMlgGameInstance::JoinSession(ULocalPlayer* localPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	check(localPlayer && localPlayer->IsValidLowLevel());

	IOnlineSessionPtr Sessions = findOnlineSession();
	onJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(onJoinSessionCompleteDelegate);

	return Sessions->JoinSession(*localPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
}

void UMlgGameInstance::onJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(onJoinSessionCompleteDelegateHandle);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onJoinSessionComplete: Session join failed"));
	}

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
	if (GetGameSessionState() == EOnlineSessionState::NoSession)
	{
		GetWorld()->ServerTravel(MAIN_MENU_MAP);
		return;
	}

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
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onDestroySessionComplete: Session destroy failed"));
	}
}

EOnlineSessionState::Type UMlgGameInstance::GetGameSessionState() const
{
	IOnlineSessionPtr Sessions = findOnlineSession();
	return Sessions->GetSessionState(GameSessionName);
}

const TArray<TSharedRef<FOnlineFriend>>& UMlgGameInstance::QueryFriendList(bool bRefreshFriendsList /* = true */, bool bOnlyPlayersInGame /* = true */)
{
	if (bRefreshFriendsList)
	{
		IOnlineFriendsPtr OnlineFriends = findOnlineFriends();

		if (!OnlineFriends->ReadFriendsList(0, DefaultFriendsList))
		{
			UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::QueryFriendList(): ReadFriendsList Failed"));
		}
		if (!OnlineFriends->GetFriendsList(0, DefaultFriendsList, friendList))
		{
			UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::QueryFriendList(): GetFriendsList Failed"));
		}
		if (bOnlyPlayersInGame)
		{
			friendList.RemoveAllSwap([](const decltype(friendList)::ElementType& element) {
				return !element->GetPresence().bIsPlayingThisGame;
			});
		}

	}

	for (int i = 0; i < friendList.Num();++i)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Index %i: Name: %s"), *friendList[i]->GetDisplayName()));
	}

	return friendList;
}

void UMlgGameInstance::JoinFriend(const FUniqueNetId& FriendToJoin)
{
	IOnlineSessionPtr session = findOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	session->FindFriendSession(0, FriendToJoin);
}

void UMlgGameInstance::onFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult)
{
	IOnlineSessionPtr session = findOnlineSession();
	session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);

	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onFindFriendSessionComplete(): Find Friend Session Failed"));
		return;
	}

	if (SearchResult.Num() < 1)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onFindFriendSessionComplete(): SearchResult Empty"));
		return;
	}

	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	JoinSession(localPlayer, SearchResult[0]);
}

void UMlgGameInstance::InviteFriend(const FUniqueNetId& FriendToInvite)
{
	IOnlineSessionPtr session = findOnlineSession();

	if (!session->SendSessionInviteToFriend(0, GameSessionName, FriendToInvite))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::InviteFriend(): SendSessionInviteToFriend Failed"));
	}
}

void UMlgGameInstance::InviteFriend(int32 IndexInLastReturnedFriendlist)
{
	if (IndexInLastReturnedFriendlist < friendList.Num())
	{
		InviteFriend(*friendList[IndexInLastReturnedFriendlist]->GetUserId());
	}
}
