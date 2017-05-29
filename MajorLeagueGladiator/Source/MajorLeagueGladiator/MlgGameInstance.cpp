// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameInstance.h"

namespace
{
	const FString PRE_BEGIN_MAP("/Game/DoubleCircle?game=Class'/Script/RailGunner.RailGunnerGameMode'?listen");
}


UMlgGameInstance::UMlgGameInstance(const FObjectInitializer & ObjectInitializer)
{
	m_onCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onCreateSessionComplete);
	m_onStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onStartSessionComplete);
	m_onFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onFindSessionsComplete);
	m_onJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onJoinSessionComplete);
	m_onDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMlgGameInstance::onDestroySessionComplete);
}

bool UMlgGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(DebugLog, Error, TEXT("No OnlineSubsytem found!"));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return false;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid() || !UserId.IsValid())
	{
		UE_LOG(DebugLog, Error, TEXT("Session or UserId not valid"));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Session or UserId not valid"));
		return false;
	}
	/*
	Fill in all the Session Settings that we want to use.
	There are more with SessionSettings.Set(...);
	For example the Map or the GameMode/Type.
	*/

	m_sessionSettings = MakeShareable(new FOnlineSessionSettings());
	if (!m_sessionSettings.IsValid())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("m_sessionSettings invalid"));
		return false;
	}
	m_sessionSettings->bIsLANMatch = bIsLan;
	m_sessionSettings->bUsesPresence = bIsPresence;
	m_sessionSettings->NumPublicConnections = MaxNumPlayers;
	m_sessionSettings->NumPrivateConnections = 0;
	m_sessionSettings->bAllowInvites = true;
	m_sessionSettings->bAllowJoinInProgress = true;
	m_sessionSettings->bShouldAdvertise = true;
	m_sessionSettings->bAllowJoinViaPresence = true;
	m_sessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	//m_sessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);

	// Set the delegate to the Handle of the SessionInterface
	m_onCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(m_onCreateSessionCompleteDelegate);

	// Our delegate should get called when this is complete (doesn't need to be successful!)
	return Sessions->CreateSession(*UserId, GameSessionName, *m_sessionSettings);


}

void UMlgGameInstance::onCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		return;
	}
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(m_onCreateSessionCompleteDelegateHandle);
	if (!bWasSuccessful)
	{
		return;
	}
	// Set the StartSession delegate handle
	m_onStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(m_onStartSessionCompleteDelegate);

	// Our StartSessionComplete delegate should get called after this
	Sessions->StartSession(SessionName);
}

void UMlgGameInstance::onStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub == nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		return;
	}
	Sessions->ClearOnStartSessionCompleteDelegate_Handle(m_onStartSessionCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("change to double Circle map"));
		GetWorld()->ServerTravel(PRE_BEGIN_MAP);
	}
}

void UMlgGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence, int32 MaxSearchResults, int32 PingBucketSize)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub == nullptr)
	{
		onFindSessionsComplete(false);
		return;
	}
	// Get the SessionInterface from our OnlineSubsystem
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	if (!Sessions.IsValid() || !UserId.IsValid())
	{
		return;
	}
	/*
	Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
	*/
	m_sessionSearch = MakeShareable(new FOnlineSessionSearch());

	m_sessionSearch->bIsLanQuery = bIsLAN;
	m_sessionSearch->MaxSearchResults = MaxSearchResults;
	m_sessionSearch->PingBucketSize = PingBucketSize;

	// We only want to set this Query Setting if "bIsPresence" is true
	if (bIsPresence)
	{
		m_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = m_sessionSearch.ToSharedRef();

	// Set the Delegate to the Delegate Handle of the FindSession function
	m_onFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(m_onFindSessionsCompleteDelegate);

	// Finally call the SessionInterface function. The Delegate gets called once this is finished
	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

void UMlgGameInstance::onFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub == nullptr)
	{
		onFindSessionsComplete(false);
		return;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		return;
	}

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(m_onFindSessionsCompleteDelegateHandle);

	OnFindSessionFinished.Broadcast(m_sessionSearch->SearchResults);
}

bool UMlgGameInstance::JoinSession(ULocalPlayer* localPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		return false;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid() || !localPlayer->IsValidLowLevel())
	{
		return false;
	}
	// Set the Handle again
	m_onJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(m_onJoinSessionCompleteDelegate);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Join Session"));
	// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
	// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
	return Sessions->JoinSession(*localPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
}

void UMlgGameInstance::onJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		return;
	}
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	if (!Sessions.IsValid())
	{
		return;
	}

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(m_onJoinSessionCompleteDelegateHandle);

	// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
	// This is something the Blueprint Node "Join Session" does automatically!
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
	// String for us by giving him the SessionName and an empty String. We want to do this, because
	// Every OnlineSubsystem uses different TravelURLs
	FString TravelURL;

	if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
	{
		// Finally call the ClienTravel. If you want, you could print the TravelURL to see
		// how it really looks like
		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}


}

void UMlgGameInstance::onDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		return;
	}

	// Get the SessionInterface from the OnlineSubsystem
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		return;
	}

	// Clear the Delegate
	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(m_onDestroySessionCompleteDelegateHandle);

	// If it was successful, we just load another level (could be a MainMenu!)
	if (bWasSuccessful)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
	}


}

