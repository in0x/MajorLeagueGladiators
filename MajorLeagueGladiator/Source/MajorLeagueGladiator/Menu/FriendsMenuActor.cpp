// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "FriendsMenuActor.h"
#include "MlgGameInstance.h"
#include "WidgetComponent.h"
#include "FriendListWidget.h"
#include "SteamBridge.h"

AFriendsMenuActor::AFriendsMenuActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, menuLocation(EFriendsMenuLocation::MainMenu)
{
	PrimaryActorTick.bCanEverTick = false;

	widgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	ConstructorHelpers::FClassFinder<UUserWidget> friendlistWidgetFinder(TEXT("/Game/BluePrints/Menu/FriendlistWidgetBP"));
	widgetComponent->SetWidgetClass(friendlistWidgetFinder.Class);
}

void AFriendsMenuActor::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* result = GetGameInstance();
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(result);

	if (!GEngine->IsEditor())
	{
		gameInstance->OnFriendlistRead.AddUObject(this, &AFriendsMenuActor::OnFriendlistLoaded);
		gameInstance->ReadFriendList();
	}
	else
	{
		UE_LOG(DebugLog, Log, TEXT("Using mock fetch friends in editor to prevent crash"));
		MockFriendsListLoadedEditor();
	}

	UFriendlistWidget* listWidget = CastChecked<UFriendlistWidget>(widgetComponent->GetUserWidgetObject());
	listWidget->FriendsRefreshRequest.AddDynamic(this, &AFriendsMenuActor::OnFriendsRefreshRequested);

	onFindFriendSessionCompleteDelegate = FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &AFriendsMenuActor::OnFindFriendSessionComplete);
}

void AFriendsMenuActor::OnFriendsRefreshRequested()
{
	UGameInstance* result = GetGameInstance();
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(result);

	gameInstance->OnFriendlistRead.AddUObject(this, &AFriendsMenuActor::OnFriendlistLoaded);
	gameInstance->ReadFriendList();
}

void AFriendsMenuActor::OnFriendsInfoLoaded_Implementation(const TArray<FFriendState>& friends)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("OnFriendsInfoLoaded_Implementation called"));
}

void AFriendsMenuActor::UpdateAvatarTextureCount(int32 CurrentNumFriends)
{
	int32 numMissingTex = CurrentNumFriends - avatarTextures.Num();

	if (numMissingTex <= 0)
	{
		return;
	}

	for (int32 i = 0; i < numMissingTex; ++i)
	{
		avatarTextures.Add(nullptr);
	}
}

void AFriendsMenuActor::MockFriendsListLoadedEditor()
{
	TArray<FFriendState> friendStates;
	int32 numFriends = 15;

	UpdateAvatarTextureCount(numFriends);

	for (int32 i = 0; i < 5; ++i)
	{
		FString name = FString::Printf(TEXT("Friend#%dNameTooLongBoiiii12345689"), numFriends);
		friendStates.Emplace(name, nullptr, i, EOnlineState::InGame, false, true);
	}

	for (int32 i = 5; i < 10; ++i)
	{
		FString name = FString::Printf(TEXT("Friend#%dNameTooLongBoiiii12345689"), numFriends);
		friendStates.Emplace(name, nullptr, i, EOnlineState::Online, false, true);
	}

	for (int32 i = 10; i < 15; ++i)
	{
		FString name = FString::Printf(TEXT("Friend#%dNameTooLongBoiiii12345689"), numFriends);
		friendStates.Emplace(name, nullptr, i, EOnlineState::Offline, false, false);
	}

	friendStates.Sort([](const FFriendState& lhs, const FFriendState& rhs) { return lhs.onlineState < rhs.onlineState; });
	OnFriendsInfoLoaded(friendStates);
}

void AFriendsMenuActor::OnFriendlistLoaded(const TArray<TSharedRef<FOnlineFriend>>& friendlist)
{
	TArray<FFriendState> friendStates;
	int32 numFriends = friendlist.Num();

	UpdateAvatarTextureCount(numFriends);

	USteamBridge* steam = nullptr;
	if (!GEngine->IsEditor())
	{
		steam = USteamBridge::Get();
		check(steam);
	}

	for (int32 i = 0; i < numFriends; ++i)
	{
		FString name = friendlist[i]->GetDisplayName();
		const FOnlineUserPresence& presence = friendlist[i]->GetPresence();
		bool bIsOnline = presence.bIsOnline;
		bool bIsPlaying = presence.bIsPlayingThisGame;
		bool bCanJoin = presence.bIsJoinable;

		bool bCanBeInvited = bIsOnline || bIsPlaying;
		EOnlineState::Type state;

		if (bIsPlaying)
		{
			state = EOnlineState::InGame;

			if (!GEngine->IsEditor())
			{
				FindFriendSession(friendlist[i]);
			}
			else
			{
				UE_LOG(DebugLog, Log, TEXT("Not checking joinable status in editor to prevent crash"));
			}
		}
		else if (!bIsPlaying && bIsOnline)
		{
			state = EOnlineState::Online;
		}
		else
		{
			state = EOnlineState::Offline;
		}

		UTexture2D* avatar = avatarTextures[i];
		if (!GEngine->IsEditor())
		{
			bool bAvatarAvailable = steam->RequestAvatarData(i); // We need to defer this
			
			if (bAvatarAvailable)
			{
				if (nullptr == avatar)
				{
					avatar = steam->CreateAvatarTexture(i);
				}

				avatarTextures[i] = avatar;
				steam->LoadAvatarData(i, avatar);
			}
		}

		friendStates.Emplace(name, avatar, i, state, bCanJoin, bCanBeInvited);
	}

	friendStates.Sort([](const FFriendState& lhs, const FFriendState& rhs) { return lhs.onlineState < rhs.onlineState; });

	OnFriendsInfoLoaded(friendStates);
}

void AFriendsMenuActor::OnAvatarDownloaded(int32 FriendIndex)
{
	UpdateAvatarTextureCount(FriendIndex);

	USteamBridge* steam = USteamBridge::Get();

	UTexture2D* avatar = avatarTextures[FriendIndex];
	if (nullptr == avatar)
	{
		avatar = steam->CreateAvatarTexture(FriendIndex);
		avatarTextures[FriendIndex] = avatar;
	}

	steam->LoadAvatarData(FriendIndex, avatar);

	UFriendWidget* widget = *friendWidgets.FindByPredicate([&](const UFriendWidget* widget) {return widget->GetFriendIndex() == FriendIndex; });
	widget->ChangeAvatar(avatar);
}

void AFriendsMenuActor::PostWidgetsConstructed()
{
	// NOTE(Phil): We run this again after constructing the widgets
	// to check if either the avatars have been fetched after first
	// requesting in OnFriendlistLoaded() or else we wait on the 
	// fetch by listening to the AvatarDownloaded callback. We only 
	// listen for this after constructing the widgets as it would else
	// be possible to be interupted by the callback while the widgets 
	// are still being constructed.

	if (!GEngine->IsEditor())
	{
		USteamBridge* steam = USteamBridge::Get();
		check(steam);
		
		if (steam->AvatarDownloaded.IsBound())
		{
			steam->AvatarDownloaded.AddDynamic(this, &AFriendsMenuActor::OnAvatarDownloaded);
		}

		for (UFriendWidget* widget : friendWidgets)
		{
			int32 friendIndex = widget->GetFriendIndex();
			UTexture2D* avatar = avatarTextures[friendIndex];
		
			bool bAvatarAvailable = steam->RequestAvatarData(friendIndex);

			if (bAvatarAvailable)
			{
				if (nullptr == avatar)
				{
					avatar = steam->CreateAvatarTexture(friendIndex);
				}

				avatarTextures[friendIndex] = avatar;
				steam->LoadAvatarData(friendIndex, avatar);
				widget->ChangeAvatar(avatar);
			}	
		}
	}
}

void AFriendsMenuActor::OnJoinFriendRequest(int32 friendIndex)
{
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(GetGameInstance());
	gameInstance->JoinFriend(friendIndex);
}

void AFriendsMenuActor::OnInviteFriendRequest(int32 friendIndex)
{
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(GetGameInstance());
	gameInstance->InviteFriend(friendIndex);
}

void AFriendsMenuActor::BindToFriendWidget(UFriendWidget* Widget)
{
	Widget->JoinFriendRequested.AddDynamic(this, &AFriendsMenuActor::OnJoinFriendRequest);
	Widget->InviteFriendRequested.AddDynamic(this, &AFriendsMenuActor::OnInviteFriendRequest);
}

void AFriendsMenuActor::HideUnusedWidgets(int32 LastUsedIndex)
{
	if (LastUsedIndex >= friendWidgets.Num())
	{
		return;
	}

	int32 numWidgets = friendWidgets.Num();
	for (int i = LastUsedIndex; i < numWidgets; ++i)
	{
		friendWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

IOnlineSessionPtr GetOnlineSession()
{
	IOnlineSubsystem* online = IOnlineSubsystem::Get();
	check(online);
	IOnlineSessionPtr session = online->GetSessionInterface();
	check(session.IsValid());
	return session;
}

void AFriendsMenuActor::FindFriendSession(TSharedRef<FOnlineFriend> Friend)
{
	IOnlineSessionPtr session = GetOnlineSession();

	onFindFriendSessionCompleteDelegateHandle = session->AddOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegate);
	if (!session->FindFriendSession(0, *Friend->GetUserId()))
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::JoinFirstAvailableFriend(): Start Find Friend Session Failed"));
		session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);
	}
}

void AFriendsMenuActor::OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult)
{
	IOnlineSessionPtr session = GetOnlineSession();

	session->ClearOnFindFriendSessionCompleteDelegate_Handle(0, onFindFriendSessionCompleteDelegateHandle);

	if (!bWasSuccessful)
	{
		UE_LOG(DebugLog, Warning, TEXT("UMlgGameInstance::onFindFriendSessionComplete(): Find Friend Session Failed"));
		return;
	}

	if (SearchResult.Num() < 1)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, *FString::Printf(TEXT("Found session for user %s"), *SearchResult[0].Session.OwningUserName));

	UFriendWidget* widget = *friendWidgets.FindByPredicate([&](const UFriendWidget* widget) { return widget->GetUsername() == SearchResult[0].Session.OwningUserName; });
	check(widget != nullptr);
	widget->ChangeCanJoin(true);
}