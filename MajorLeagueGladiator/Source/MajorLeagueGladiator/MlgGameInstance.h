// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"

#include "MlgGameInstance.generated.h"

//Used Tutorial : https://wiki.unrealengine.com/How_To_Use_Sessions_In_C%2B%2B


DECLARE_MULTICAST_DELEGATE_OneParam(SearchFinishedDelegate, const TArray <FOnlineSessionSearchResult>&);

struct WaveSystemSavedState
{
	int32 currentWaveNumber;
	int32 startWaveNumber;
	int32 remainingEnemies;
};

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMlgGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMlgGameInstance(const FObjectInitializer& ObjectInitializer);

	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence, int32 MaxSearchResults = 20, int32 PingBucketSize = 50);
	virtual bool JoinSession(ULocalPlayer* localPlayer, const FOnlineSessionSearchResult& SearchResult) override;

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

	FOnCreateSessionCompleteDelegate  onCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate  onStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate onFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate onJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate onDestroySessionCompleteDelegate;

	FDelegateHandle onCreateSessionCompleteDelegateHandle;
	FDelegateHandle onStartSessionCompleteDelegateHandle;
	FDelegateHandle onFindSessionsCompleteDelegateHandle;
	FDelegateHandle onJoinSessionCompleteDelegateHandle;
	FDelegateHandle onDestroySessionCompleteDelegateHandle;
	
	TSharedPtr<class FOnlineSessionSettings> sessionSettings;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
};
