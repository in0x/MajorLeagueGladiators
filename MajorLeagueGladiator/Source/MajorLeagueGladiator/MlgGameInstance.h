// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"

#include "MlgGameInstance.generated.h"

//Used Tutorial : https://wiki.unrealengine.com/How_To_Use_Sessions_In_C%2B%2B


DECLARE_MULTICAST_DELEGATE_OneParam(SearchFinishedDelegate, const TArray <FOnlineSessionSearchResult>&);

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

	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence, int32 MaxSearchResults = 20, int32 PingBucketSize = 50);

	virtual bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) override;
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) override;

	void ReadFriendList();
	
	void JoinFirstAvailableFriend();
	void InviteFirstAvailableFriend();


	void TravelToMainMenu();

	SearchFinishedDelegate OnFindSessionFinished;

	EOnlineSessionState::Type GetGameSessionState() const;

	WaveSystemSavedState WaveSystemSavedState;
	bool bIsInRoomOfShame;

private:

	virtual void onCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void onStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void onFindSessionsComplete(bool bWasSuccessful);
	void onJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void onDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void onFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult);

	//void onSessionUserInviteAccepted(bool bWasSuccessful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	void onReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	
	FOnCreateSessionCompleteDelegate  onCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate  onStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate onFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate onJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate onDestroySessionCompleteDelegate;
	FOnFindFriendSessionCompleteDelegate onFindFriendSessionCompleteDelegate;
	//FOnSessionUserInviteAcceptedDelegate onSessionUserInviteAcceptedDelegate;

	FDelegateHandle onCreateSessionCompleteDelegateHandle;
	FDelegateHandle onStartSessionCompleteDelegateHandle;
	FDelegateHandle onFindSessionsCompleteDelegateHandle;
	FDelegateHandle onJoinSessionCompleteDelegateHandle;
	FDelegateHandle onDestroySessionCompleteDelegateHandle;
	FDelegateHandle onFindFriendSessionCompleteDelegateHandle;
	//FDelegateHandle onSessionUserInviteAcceptedDelegateHandle;

	FOnReadFriendsListComplete onReadFriendsListCompleteDelegate;
	
	TSharedPtr<class FOnlineSessionSettings> sessionSettings;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
	TArray<TSharedRef<FOnlineFriend>> friendList;
};
