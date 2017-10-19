// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "FriendsMenuActor.h"
#include "MlgGameInstance.h"
#include "WidgetComponent.h"
#include "FriendListWidget.h"

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

			if (!GEngine->IsEditor())
			{
				FindFriendSession(friendlist[i]);
			}
			else
			{
				UE_LOG(DebugLog, Log, TEXT("Not checing joinable status in editor to prevent crash"));
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

		friendStates.Emplace(name, i, state, bCanJoin);
	}

	friendStates.Sort([](const FFriendState& lhs, const FFriendState& rhs) { return lhs.onlineState < rhs.onlineState; });

	OnFriendsInfoLoaded(friendStates);	
}

void AFriendsMenuActor::OnJoinFriendRequest(int32 friendIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("OnJoinFriendRequested called"));

	UGameInstance* result = GetGameInstance();
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(result);
	gameInstance->JoinFriend(friendIndex);
}

void AFriendsMenuActor::BindToFriendWidget(UFriendWidget* Widget)
{
	Widget->JoinFriendRequested.AddDynamic(this, &AFriendsMenuActor::OnJoinFriendRequest);
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
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	check(onlineSub);
	IOnlineSessionPtr session = onlineSub->GetSessionInterface();
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