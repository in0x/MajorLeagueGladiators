// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online.h"
#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MlgGameSession.generated.h"


struct FMlgSessionParams
{
	/** Name of session settings are stored with */
	FName SessionName;
	/** LAN Match */
	bool bIsLAN;
	/** Presence enabled session */
	bool bIsPresence;
	/** Id of player initiating lobby */
	TSharedPtr<const FUniqueNetId> UserId;

	FMlgSessionParams()
		: SessionName(NAME_None)
		, bIsLAN(false)
		, bIsPresence(false)
	{
	}
};
/**
 * More or less copied from Shooter Game (ShooterGameSession.h)
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameSession : public AGameSession
{
	GENERATED_BODY()
public:
	AMlgGameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:
	/** Delegate for creating a new session */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/** Delegate after starting a session */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Transient properties of a session during game creation/matchmaking */
	FMlgSessionParams CurrentSessionParams;
	/** Current host settings */
	TSharedPtr<class FOnlineSessionSettings> HostSettings;
	/** Current search settings */
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;

	/**
	* Delegate fired when a session create request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when a session start request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when a session search query has completed
	*
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	* Delegate fired when a session join request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	* Called when this instance is starting up as a dedicated server
	*/
	virtual void RegisterServer() override;


public:

	/*
	* Event triggered when a presence session is created
	*
	* @param SessionName name of session that was created
	* @param bWasSuccessful was the create successful
	*/
	DECLARE_EVENT_TwoParams(MlgGameSession, FOnCreateSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreateSessionComplete CreateSessionCompleteEvent;

	/*
	* Event triggered when a session is joined
	*
	* @param SessionName name of session that was joined
	* @param bWasSuccessful was the create successful
	*/
	//DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnJoinSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	DECLARE_EVENT_OneParam(MlgGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/*
	* Event triggered after session search completes
	*/
	DECLARE_EVENT_OneParam(MlgGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;


	/**
	* Host a new online session
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session
	* @param bIsLAN is this going to hosted over LAN
	* @param bIsPresence is the session to create a presence session
	* @param MaxNumPlayers Maximum number of players to allow in the session
	*
	* @return bool true if successful, false otherwise
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);

	/**
	* Find an online session
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session this search will generate
	* @param bIsLAN are we searching LAN matches
	* @param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);

	/**
	* Joins one of the session in search results
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session
	* @param SessionIndexInSearchResults Index of the session in search results
	*
	* @return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults);

	/**
	* Joins a session via a search result
	*
	* @param SessionName name of session
	* @param SearchResult Session to join
	*
	* @return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** @return true if any online async work is in progress, false otherwise */
	bool IsBusy() const;

	const TArray<FOnlineSessionSearchResult> & GetSearchResults() const;

	/** Handle starting the match */
	virtual void HandleMatchHasStarted() override;

	/** Handles when the match has ended */
	virtual void HandleMatchHasEnded() override;

	/** Handles to various registered delegates */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;	
};
