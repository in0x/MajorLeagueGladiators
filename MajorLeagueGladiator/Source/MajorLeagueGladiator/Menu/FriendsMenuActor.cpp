// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "FriendsMenuActor.h"
#include "MlgGameInstance.h"
#include "WidgetComponent.h"

AFriendsMenuActor::AFriendsMenuActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
		UE_LOG(DebugLog, Log, TEXT("Not fetching friends in editor to prevent crash"));
	}
}

void AFriendsMenuActor::OnFriendsInfoLoaded_Implementation(const TArray<FFriendState>& friends)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("OnFriendsInfoLoaded_Implementation called"));
}

void AFriendsMenuActor::OnFriendlistLoaded(const TArray<TSharedRef<FOnlineFriend>>& friendlist)
{
	TArray<FFriendState> friendStates;
	int32 numFriends = friendlist.Num();

	friendStates.Emplace(TEXT("TestInGame"), EOnlineState::InGame, true);
	friendStates.Emplace(TEXT("TestOnline"), EOnlineState::Online, false);
	friendStates.Emplace(TEXT("TestOffline"), EOnlineState::Offline, false);

	for (int32 i = 0; i < numFriends; ++i)
	{
		FString name = friendlist[i]->GetDisplayName();
		const FOnlineUserPresence& presence = friendlist[i]->GetPresence();
		bool bIsOnline = presence.bIsOnline;
		bool bIsPlaying = presence.bIsPlayingThisGame;
		bool bCanJoin = presence.bIsJoinable;

		EOnlineState::Type state;

		if (bIsPlaying)
		{
			state = EOnlineState::InGame;
		}
		else if (!bIsPlaying && bIsOnline)
		{
			state = EOnlineState::Online;
		}
		else
		{
			state = EOnlineState::Offline;
		}

		friendStates.Emplace(name, state, bCanJoin);
	}

	OnFriendsInfoLoaded(friendStates);	
}

void AFriendsMenuActor::OnJoinFriendRequest(int32 friendIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("OnJoinFriendRequested called but not implemented yet."));
}

void AFriendsMenuActor::BindToFriendWidget(UFriendWidget* Widget)
{
	Widget->JoinFriendRequested.AddDynamic(this, &AFriendsMenuActor::OnJoinFriendRequest);
}

