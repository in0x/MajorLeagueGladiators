// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"

#include "MlgGameInstance.generated.h"

//Used Tutorial : https://wiki.unrealengine.com/How_To_Use_Sessions_In_C%2B%2B

class AMlgGameSession;

DECLARE_MULTICAST_DELEGATE_OneParam(SearchFinishedDelegate, const TArray<FOnlineSessionSearchResult>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FriendlistReadFinishedDelegate, const TArray<TSharedRef<FOnlineFriend>>&);

struct WaveSystemSavedState
{
	int32 currentWaveNumber = 1;
	int32 startWaveNumber = 1;
	int32 remainingEnemies = 0;
};

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMlgGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMlgGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;
	virtual void Shutdown() override;

	AMlgGameSession* GetGameSession() const;
	void AddNetworkFailureHandlers();
	void RemoveNetworkFailureHandlers();
	void OnTravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString);
	bool HostSession(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);
	bool FindSessions(ULocalPlayer* PlayerOwner, bool bFindLan);

	virtual bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) override;
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) override;

	void ReadFriendList();
	
	void JoinFriend(int32 friendlistIndex);
	void JoinFirstAvailableFriend();
	void InviteFirstAvailableFriend();

	void TravelToMainMenu();

	SearchFinishedDelegate OnFindSessionFinished;

	FriendlistReadFinishedDelegate OnFriendlistRead;

	EOnlineSessionState::Type GetGameSessionState() const;

	WaveSystemSavedState WaveSystemSavedState;
	bool bIsInRoomOfShame;

private:

	void onCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void onFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);
	void onDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void onDestroySessionCompleteWhenShutdown(FName SessionName, bool bWasSuccessful);
	void onFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult);

	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	FOnDestroySessionCompleteDelegate onDestroySessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate onDestroySessionCompleteWhenShutdownDelegate;
	FOnFindFriendSessionCompleteDelegate onFindFriendSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate onStartSessionCompleteDelegate;

	FDelegateHandle onCreateSessionCompleteDelegateHandle;
	FDelegateHandle onStartSessionCompleteDelegateHandle;
	FDelegateHandle onFindSessionsCompleteDelegateHandle;
	FDelegateHandle onJoinSessionCompleteDelegateHandle;
	FDelegateHandle onDestroySessionCompleteDelegateHandle;
	FDelegateHandle onDestroySessionCompleteWhenShutdownDelegateHandle;
	FDelegateHandle onFindFriendSessionCompleteDelegateHandle;
	FDelegateHandle onTravelLocalSessionFailureDelegateHandle;

	FOnReadFriendsListComplete onReadFriendsListCompleteDelegate;
	
	TArray<TSharedRef<FOnlineFriend>> friendList;
};
