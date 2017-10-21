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
}

void UMlgGameInstance::Shutdown()
{
	Super::Shutdown();

	IOnlineSessionPtr Sessions = findOnlineSession();

	if (Sessions.IsValid())
	{
		onDestroySessionCompleteWhenShutdownDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteWhenShutdownDelegate);

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
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			UE_LOG(DebugLog, Error, TEXT("JoinSessionFailed - Game no longer exists"));
			break;
		default:
			UE_LOG(DebugLog, Error, TEXT("JoinSessionFailed"));
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
		UE_LOG(DebugLog, Warning, TEXT("Failed to travel to session upon joining it"));
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Inviting: %s"), *friendList[IndexOfFirstAvailableFriend]->GetDisplayName()));

	IOnlineSessionPtr session = findOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	if (!session->FindFriendSession(0, *friendList[IndexOfFirstAvailableFriend]->GetUserId()))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::JoinFirstAvailableFriend(): Start Find Friend Session Failed"));
		session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);
	}
}

//#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/isteamuser.h"

//#include "steam/steam_api.h"
//#include "steam/isteamuser.h"

UTexture2D* UMlgGameInstance::GetProfilePicture()
{
	uint32 width;
	uint32 height;

	if (SteamAPI_Init())
	{
		//Getting the PictureID from the SteamAPI and getting the Size with the ID
		int Picture = SteamFriends()->GetMediumFriendAvatar(SteamUser()->GetSteamID());
		SteamUtils()->GetImageSize(Picture, &width, &height);

		if (width > 0 && height > 0)
		{
			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			BYTE *oAvatarRGBA = new BYTE[width * height * 4];

			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * height * width * sizeof(char));

			//Swap R and B channels because for some reason the games whack
			for (uint32 i = 0; i < (width * height * 4); i += 4)
			{
				uint8 Temp = oAvatarRGBA[i + 0];
				oAvatarRGBA[i + 0] = oAvatarRGBA[i + 2];
				oAvatarRGBA[i + 2] = Temp;
			}

			UTexture2D* Avatar = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8);

			//MAGIC!
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, height * width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();

			return Avatar;
		}
		return nullptr;
	}
	return nullptr;
}
