// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "OnlineSessionSettings.h"
#include "UniqueNetIdWrapper.h"

#include "MenuGameMode.generated.h"

namespace EMenuAction { enum Type; }
class UMlgGameInstance;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMenuGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMenuGameMode();
	virtual	TSubclassOf<AGameSession> GetGameSessionClass() const override;
	virtual void BeginPlay() override;

private:
	void startRangedTutorial();
	void startMeleeTutorial();
	void hostGame();
	void findGame();
	void joinGame(const FOnlineSessionSearchResult& searchResultToJoin);
	void onGamesFound(const TArray <FOnlineSessionSearchResult>& foundGames);
	void onMenuAction(TEnumAsByte<EMenuAction::Type> menuAction);
	void setTutorialSettings();

	UMlgGameInstance* getMlgGameInstance() const;
	TSharedPtr<const FUniqueNetId> getUniqueNetID() const;
};
