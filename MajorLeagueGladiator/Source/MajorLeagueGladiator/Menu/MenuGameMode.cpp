// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuGameMode.h"

#include "Menu/MenuCharacter.h"
#include "MlgGameInstance.h"

namespace
{
	const FString RANGED_TUTORIAL_MAP("/Game/DPS_Tutorial?game=/Game/Tutorial/DPSTutorialGamemode");
	const FString MELEE_TUTORIAL_MAP("/Game/Tank_Tutorial?game=/Game/Tutorial/TankTutorialGamemode");
}

AMenuGameMode::AMenuGameMode()
{
	DefaultPawnClass = AMenuCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	//PlayerStateClass = AMlgPlayerState::StaticClass();
	//GameStateClass = AMlgGameState::StaticClass();
	//fxManagerClass = AEffectsManagerActor::StaticClass();
}

void AMenuGameMode::startRangedTutorial()
{
	GetWorld()->ServerTravel(RANGED_TUTORIAL_MAP);
}

void AMenuGameMode::startMeleeTutorial()
{
	GetWorld()->ServerTravel(MELEE_TUTORIAL_MAP);
}

void AMenuGameMode::hostGame()
{
	getMlgGameInstance()->HostSession(getUniqueNetID(), true, true, 2);
}

void AMenuGameMode::findGame()
{
	UMlgGameInstance* gameInstance = getMlgGameInstance();

	gameInstance->OnFindSessionFinished.AddUObject(this, &AMenuGameMode::onGamesFound);
	gameInstance->FindSessions(getUniqueNetID(), true, true);
}

void AMenuGameMode::joinGame(const FOnlineSessionSearchResult& searchResultToJoin)
{
	ULocalPlayer* localplayer = GetWorld()->GetFirstLocalPlayerFromController();
	getMlgGameInstance()->JoinSession(localplayer, searchResultToJoin);
}

void AMenuGameMode::onGamesFound(const TArray <FOnlineSessionSearchResult>& foundGames)
{
	UMlgGameInstance* gameInstance = getMlgGameInstance();

	gameInstance->OnFindSessionFinished.RemoveAll(this);
	if (foundGames.Num() > 0)
	{
		joinGame(foundGames[0]);
	}
}

UMlgGameInstance* AMenuGameMode::getMlgGameInstance() const
{
	UGameInstance* result = GetGameInstance();
	return CastChecked<UMlgGameInstance>(result);
}

TSharedPtr<const FUniqueNetId> AMenuGameMode::getUniqueNetID() const
{
	ULocalPlayer* localplayer = GetWorld()->GetFirstLocalPlayerFromController();
	check(localplayer);
	auto playerId = localplayer->GetPreferredUniqueNetId();
	return playerId;
}
