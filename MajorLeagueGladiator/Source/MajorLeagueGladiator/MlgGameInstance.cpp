// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameInstance.h"
#include "MlgGameSession.h"

namespace
{
	const FString PRE_BEGIN_MAP("/Game/PreGame?listen");
	const FString MAIN_MENU_MAP("/Game/MainMenu");
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
		IOnlineFriendsPtr Sessions = getOnlineSubsystem()->GetFriendsInterface();
		check(Sessions.IsValid());
		return Sessions;
	}

}

UMlgGameInstance::UMlgGameInstance(const FObjectInitializer & ObjectInitializer)
{}

void UMlgGameInstance::Init()
{
	Super::Init();

	onDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onDestroySessionComplete);
	onDestroySessionCompleteWhenShutdownDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onDestroySessionCompleteWhenShutdown);
	onFindFriendSessionCompleteDelegate = FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onFindFriendSessionComplete);
	onReadFriendsListCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UMlgGameInstance::OnReadFriendsListComplete);
	onSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UMlgGameInstance::OnSessionUserInviteAccepted);

	onSessionUserInviteAcceptedDelegateHandle = findOnlineSession()->AddOnSessionUserInviteAcceptedDelegate_Handle(onSessionUserInviteAcceptedDelegate);
}

void UMlgGameInstance::Shutdown()
{
	Super::Shutdown();

	IOnlineSessionPtr Sessions = findOnlineSession();

	if (Sessions.IsValid())
	{
		onDestroySessionCompleteWhenShutdownDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteWhenShutdownDelegate);
		Sessions->ClearOnSessionUserInviteAcceptedDelegate_Handle(onSessionUserInviteAcceptedDelegateHandle);

		Sessions->DestroySession(GameSessionName);
	}
}

AMlgGameSession* UMlgGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	check(World);

	AGameModeBase* const Game = World->GetAuthGameMode();
	check(Game);

	return CastChecked<AMlgGameSession>(Game->GameSession);
}

void UMlgGameInstance::AddNetworkFailureHandlers()
{
	// Add network/travel error handlers (if they are not already there)
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		onTravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UMlgGameInstance::OnTravelLocalSessionFailure);
	}
}

void UMlgGameInstance::RemoveNetworkFailureHandlers()
{
	// Remove the local session/travel failure bindings if they exist
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(onTravelLocalSessionFailureDelegateHandle);
	}
}

void UMlgGameInstance::OnTravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString)
{
	// TODO
}

bool UMlgGameInstance::HostSession(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	AMlgGameSession* const GameSession = GetGameSession();
	check(GameSession);

	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	onCreateSessionCompleteDelegateHandle = GameSession->CreateSessionCompleteEvent.AddUObject(this, &UMlgGameInstance::onCreateSessionComplete);
	if (GameSession->HostSession(localPlayer->GetPreferredUniqueNetId(), GameSessionName, bIsLan, bIsPresence, MaxNumPlayers))
	{
		return true;
	}

	return false;
}

void UMlgGameInstance::onCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	AMlgGameSession* const GameSession = GetGameSession();
	check(GameSession);

	GameSession->CreateSessionCompleteEvent.Remove(onCreateSessionCompleteDelegateHandle);

	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onCreateSessionComplete: CreateSession failed"));
		return;
	}

	bIsInRoomOfShame = true;
	GetWorld()->ServerTravel(PRE_BEGIN_MAP);
}

bool UMlgGameInstance::FindSessions(ULocalPlayer* PlayerOwner, bool bFindLan)
{
	check(PlayerOwner);

	AMlgGameSession* const GameSession = GetGameSession();
	check(GameSession);

	GameSession->FindSessionsCompleteEvent.RemoveAll(this);
	onFindSessionsCompleteDelegateHandle = GameSession->FindSessionsCompleteEvent.AddUObject(this, &UMlgGameInstance::onFindSessionsComplete);

	GameSession->FindSessions(PlayerOwner->GetPreferredUniqueNetId(), GameSessionName, bFindLan, true);

	return true;
}

void UMlgGameInstance::onFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onFindSessionsComplete: findSession failed"));
	}

	AMlgGameSession* const Session = GetGameSession();
	check(Session)

		Session->FindSessionsCompleteEvent.Remove(onFindSessionsCompleteDelegateHandle);
	OnFindSessionFinished.Broadcast(Session->GetSearchResults());
}

bool UMlgGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	AMlgGameSession* const Session = GetGameSession();
	if (Session && Session->GetSearchResults().Num() > SessionIndexInSearchResults)
	{
		return JoinSession(LocalPlayer, Session->GetSearchResults()[SessionIndexInSearchResults]);
	}
	return false;
}

bool UMlgGameInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	check(LocalPlayer);
	AMlgGameSession* const GameSession = GetGameSession();
	check(GameSession)

	AddNetworkFailureHandlers();

	onJoinSessionCompleteDelegateHandle = GameSession->JoinSessionCompleteEvent.AddUObject(this, &UMlgGameInstance::OnJoinSessionComplete);

	return GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
}

void UMlgGameInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete Called"));

	AMlgGameSession* const GameSession = GetGameSession();
	check(GameSession);

	GameSession->JoinSessionCompleteEvent.Remove(onJoinSessionCompleteDelegateHandle);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		FText ReturnReason;
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			UE_LOG(DebugLog, Error, TEXT("JoinSessionFailed - Game is full."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete JoinSessionFailed - Game is full."));
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			UE_LOG(DebugLog, Error, TEXT("JoinSessionFailed - Game no longer exists"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete JoinSessionFailed - Game no longer exists"));
			break;
		default:
			UE_LOG(DebugLog, Error, TEXT("JoinSessionFailed"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete JoinSessionFailed"));
			break;
		}

		return;
	}

	APlayerController * const PlayerController = GetFirstLocalPlayerController();
	check(PlayerController)

		FString URL;
	IOnlineSessionPtr Sessions = findOnlineSession();

	if (!Sessions.IsValid() || !Sessions->GetResolvedConnectString(GameSessionName, URL))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete Failed to travel to session upon joining it"));
		UE_LOG(DebugLog, Warning, TEXT("Failed to travel to session upon joining it"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnJoinSessionComplete Call Client Travel"));
	PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
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
		onDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteDelegate);

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

void UMlgGameInstance::onDestroySessionCompleteWhenShutdown(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = findOnlineSession();

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteWhenShutdownDelegateHandle);

	if (bWasSuccessful)
	{
		UE_LOG(DebugLog, Warning, TEXT("Successfully destroyed session while shutting down"));
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

void UMlgGameInstance::ReadFriendList()
{
	IOnlineFriendsPtr OnlineFriends = findOnlineFriends();

	if (!OnlineFriends->ReadFriendsList(0, DefaultFriendsList, onReadFriendsListCompleteDelegate))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::ReadFriendList(): Start ReadFriendsList Failed"));
	}
}

void UMlgGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onReadFriendsListComplete(): ReadFriendsList Failed %s"), *ErrorStr);
		return;
	}

	IOnlineFriendsPtr OnlineFriends = findOnlineFriends();

	friendList.Empty();
	if (!OnlineFriends->GetFriendsList(0, DefaultFriendsList, friendList))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::QueryFriendList(): GetFriendsList Failed"));
	}

	for (int i = 0; i < friendList.Num(); ++i)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Index %i: Name: %s"), i, *friendList[i]->GetDisplayName()));
	}

	OnFriendlistRead.Broadcast(friendList);
}

void UMlgGameInstance::OnSessionUserInviteAccepted(bool bWasSuccessful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogOnline, Verbose, TEXT("OnSessionUserInviteAccepted LocalUserNum: %d bSuccess: %d"), ControllerId, bWasSuccessful);
	// Don't clear invite accept delegate

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted called"));

	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted  was successful"));
		if (SearchResult.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted  Trying to join lobby"));
			ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			bool success = JoinSession(localPlayer, SearchResult);
			if (success)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted Begin Join lobby sucessful"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted Begin Join lobby failed"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted Invite returned no search result"));
			UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no search result."));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::OnSessionUserInviteAccepted  was not successful"));
	}
}

void UMlgGameInstance::JoinFirstAvailableFriend()
{
	int32 IndexOfFirstAvailableFriend = 0;
	const int32 friendlistLength = friendList.Num();

	while (IndexOfFirstAvailableFriend < friendlistLength)
	{
		if (friendList[IndexOfFirstAvailableFriend]->GetPresence().bIsPlayingThisGame)
		{
			break;
		}
		++IndexOfFirstAvailableFriend;
	}

	if (IndexOfFirstAvailableFriend >= friendlistLength)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("No Friend Found to Join"));
		return;
	}

	JoinFriend(IndexOfFirstAvailableFriend);
}

void UMlgGameInstance::JoinFriend(int32 friendlistIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Joining: %s"), *friendList[friendlistIndex]->GetDisplayName()));

	IOnlineSessionPtr session = findOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	if (!session->FindFriendSession(0, *friendList[friendlistIndex]->GetUserId()))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::JoinFirstAvailableFriend(): Start Find Friend Session Failed"));
		session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);
	}
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::onFindFriendSessionComplete(): SearchResult Empty. Player probably has no open session"));
		return;
	}
	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	JoinSession(localPlayer, SearchResult[0]);
}

// Work in Progress
void UMlgGameInstance::InviteFirstAvailableFriend()
{
	int32 IndexOfFirstAvailableFriend = 0;
	const int32 friendlistLength = friendList.Num();

	while (IndexOfFirstAvailableFriend < friendlistLength)
	{
		// we might want to invite non playing friends
		if (friendList[IndexOfFirstAvailableFriend]->GetPresence().bIsPlayingThisGame)
		{
			break;
		}
		++IndexOfFirstAvailableFriend;
	}

	if (IndexOfFirstAvailableFriend >= friendlistLength)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::InviteFirstAvailableFriend: No Friend Found to Invite"));
		return;
	}

	InviteFriend(IndexOfFirstAvailableFriend);
}

void UMlgGameInstance::InviteFriend(int32 friendlistIndex)
{
	IOnlineSessionPtr sessions = findOnlineSession();
	
	TSharedPtr<const FUniqueNetId> localId = getOnlineSubsystem()->GetIdentityInterface()->GetUniquePlayerId(0);
	AMlgGameSession* const gameSession = GetGameSession();

	sessions->SendSessionInviteToFriend(*localId, gameSession->SessionName, *friendList[friendlistIndex]->GetUserId());
}
