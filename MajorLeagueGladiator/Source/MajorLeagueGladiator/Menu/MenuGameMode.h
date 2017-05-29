// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "OnlineSessionSettings.h"
#include "UniqueNetIdWrapper.h"

#include "MenuGameMode.generated.h"


class UMlgGameInstance;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMenuGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMenuGameMode();

public:
	void startRangedTutorial();
	void startMeleeTutorial();
	void hostGame();
	void findGame();
	void joinGame(const FOnlineSessionSearchResult& searchResultToJoin);
private:
	void onGamesFound(const TArray <FOnlineSessionSearchResult>& foundGames);

	UMlgGameInstance* getMlgGameInstance() const;
	TSharedPtr<const FUniqueNetId> getUniqueNetID() const;
};
