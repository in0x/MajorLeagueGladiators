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
	
	/*ConstructorHelpers::FClassFinder<UFriendWidget> friendWidgetFinder(TEXT("/Game/BluePrints/Menu/FriendWidgetBP"));

	UFriendWidget* friendWidget = CreateWidget<TSubclassOf<UFriendWidget>>(GetGameInstance(), friendWidgetFinder.Class);

	widgetComponent->GetUserWidgetObject();
	friendWidget->*/
}

void AFriendsMenuActor::OnFriendsInfoLoaded_Implementation(const TArray<FFriendState>& friends)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("OnFriendsInfoLoaded_Implementation called"));
}

void AFriendsMenuActor::OnFriendlistLoaded(const TArray<TSharedRef<FOnlineFriend>>& friendlist)
{
	TArray<FFriendState> friendStates;

	int32 numFriends = friendlist.Num();
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
		else if (bIsOnline)
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

	/*
		TODO(Phil): For each friend:
		
		* CreateWidget call on FriendWidget
		* Set state using info
		* Add widget to scrollable widget;
	
	*/

	//

	/*int32 IndexOfFirstAvailableFriend = 0;
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Joining: %s"), *friendList[IndexOfFirstAvailableFriend]->GetDisplayName()));*/
}

