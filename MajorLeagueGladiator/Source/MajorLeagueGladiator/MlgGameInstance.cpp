// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameInstance.h"
#include "MlgGameSession.h"

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
{}

void UMlgGameInstance::Init()
{
	Super::Init();

	//onStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onStartSessionComplete);
	//onFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onFindSessionsComplete);
	onDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onDestroySessionComplete);
	onFindFriendSessionCompleteDelegate = FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onFindFriendSessionComplete);
	//onReadFriendsListCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UMlgGameInstance::onReadFriendsListComplete);

	//onSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UMlgGameInstance::onSessionUserInviteAccepted);
	//onSessionUserInviteAcceptedDelegateHandle = findOnlineSession()->AddOnSessionUserInviteAcceptedDelegate_Handle(onSessionUserInviteAcceptedDelegate);
}

void UMlgGameInstance::Shutdown()
{
	//findOnlineSession()->ClearOnSessionUserInviteAcceptedDelegate_Handle(onSessionUserInviteAcceptedDelegateHandle);

	Super::Shutdown();
}

AMlgGameSession* UMlgGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return CastChecked<AMlgGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

void UMlgGameInstance::AddNetworkFailureHandlers()
{
	// Add network/travel error handlers (if they are not already there)
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UMlgGameInstance::TravelLocalSessionFailure);
	}
}

void UMlgGameInstance::RemoveNetworkFailureHandlers()
{
	// Remove the local session/travel failure bindings if they exist
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(TravelLocalSessionFailureDelegateHandle);
	}
}

void UMlgGameInstance::TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString)
{
	// TODO
}

bool UMlgGameInstance::HostSession(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	AMlgGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		onCreateSessionCompleteDelegateHandle = GameSession->CreateSessionCompleteEvent.AddUObject(this, &UMlgGameInstance::onCreateSessionComplete);
		if (GameSession->HostSession(localPlayer->GetPreferredUniqueNetId(), GameSessionName, bIsLan, bIsPresence, MaxNumPlayers))
		{	
			return true;
		}
	}
	return false;
}

void UMlgGameInstance::onCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	AMlgGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->CreateSessionCompleteEvent.Remove(onCreateSessionCompleteDelegateHandle);
		//Session Can be started by Changing the Gamemode state to MatchState::InProgress
	}
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onCreateSessionComplete: CreateSession failed"));
	}
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

bool UMlgGameInstance::FindSessions(ULocalPlayer* PlayerOwner, bool bFindLan)
{
	bool bResult = false;

	check(PlayerOwner != nullptr);
	if (PlayerOwner)
	{
		AMlgGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			GameSession->FindSessionsCompleteEvent.RemoveAll(this);
			onFindSessionsCompleteDelegateHandle = GameSession->FindSessionsCompleteEvent.AddUObject(this, &UMlgGameInstance::onFindSessionsComplete);

			GameSession->FindSessions(PlayerOwner->GetPreferredUniqueNetId(), GameSessionName, bFindLan, true);

			bResult = true;
		}
	}
	return bResult;
}

void UMlgGameInstance::onFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Error, TEXT("UMlgGameInstance::onFindSessionsComplete: findSession failed"));
	}

	AMlgGameSession* const Session = GetGameSession();
	if (Session)
	{
		Session->FindSessionsCompleteEvent.Remove(onFindSessionsCompleteDelegateHandle);
		OnFindSessionFinished.Broadcast(Session->GetSearchResults());
	}	
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
	if (GameSession)
	{
		AddNetworkFailureHandlers();

		onJoinSessionCompleteDelegateHandle = GameSession->JoinSessionCompleteEvent.AddUObject(this, &UMlgGameInstance::onJoinSessionComplete);
		if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult))
		{
			return true;
		}
	}
	return false;
}


void UMlgGameInstance::onJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	AMlgGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->JoinSessionCompleteEvent.Remove(onJoinSessionCompleteDelegateHandle);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		FText ReturnReason;
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("JoinSessionFailed - Game is full."));
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("JoinSessionFailed - Game no longer exists"));
			break;
		default:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("JoinSessionFailed"));
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Travel to Session failed"));
		UE_LOG(LogOnlineGame, Warning, TEXT("Failed to travel to session upon joining it"));
		return;
	}

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

void UMlgGameInstance::ReadFriendList()
{
	IOnlineFriendsPtr OnlineFriends = findOnlineFriends();

	if (!OnlineFriends->ReadFriendsList(0, DefaultFriendsList, onReadFriendsListCompleteDelegate))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::ReadFriendList(): Start ReadFriendsList Failed"));
	}
}

void UMlgGameInstance::onReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onReadFriendsListComplete(): ReadFriendsList Failed %s"), *ErrorStr);
	}
	else
	{
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Joining: %s"), *friendList[IndexOfFirstAvailableFriend]->GetDisplayName()));

	IOnlineSessionPtr session = findOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	if (!session->FindFriendSession(0, *friendList[IndexOfFirstAvailableFriend]->GetUserId()))
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
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onFindFriendSessionComplete(): SearchResult Empty"));
		return;
	}
	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	JoinSession(localPlayer, SearchResult[0]);
}


void UMlgGameInstance::InviteFirstAvailableFriend()
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UMlgGameInstance::InviteFirstAvailableFriend: No Friend Found to Invite"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Inviting: %s"), *friendList[IndexOfFirstAvailableFriend]->GetDisplayName()));

	IOnlineSessionPtr session = findOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	if (!session->FindFriendSession(0, *friendList[IndexOfFirstAvailableFriend]->GetUserId()))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::JoinFirstAvailableFriend(): Start Find Friend Session Failed"));
		session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);
	}
}

// Should Already be handled by OnlineSessionClient
//void UMlgGameInstance::onSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
//{
//	if (InviteResult.IsValid())
//	{
//		ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
//		JoinSession(localPlayer,InviteResult);
//	}
//}