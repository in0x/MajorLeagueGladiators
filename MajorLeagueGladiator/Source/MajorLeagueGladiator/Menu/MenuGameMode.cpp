// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuGameMode.h"

#include "Menu/MenuCharacter.h"
#include "MlgGameInstance.h"
#include "Menu/MenuCharacter.h"
#include "MenuActionComponent.h"
#include "MlgGameSession.h"
#include "MenuActionWidget.h"

namespace
{
	const FString RANGED_TUTORIAL_MAP("/Game/DPS_Tutorial?game=/Game/Tutorial/DPSTutorialGamemode");
	const FString MELEE_TUTORIAL_MAP("/Game/Tank_Tutorial?game=/Game/Tutorial/TankTutorialGamemode");
}

AMenuGameMode::AMenuGameMode()
{
	ConstructorHelpers::FClassFinder<AMenuCharacter> menuCharacterBP(TEXT("/Game/BluePrints/Characters/MenuCharacterBP"));
	DefaultPawnClass = menuCharacterBP.Class;
}

TSubclassOf<AGameSession> AMenuGameMode::GetGameSessionClass() const
{
	return AMlgGameSession::StaticClass();
}

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (TObjectIterator<UMenuActionComponent> iter; iter; ++iter)
	{
		iter->OnMenuActionTriggered.AddUObject(this, &AMenuGameMode::onMenuAction);
	}
	
	UWorld* world = GetWorld();

	for (TObjectIterator<UMenuActionWidget> iter; iter; ++iter)
	{
		if (iter->GetWorld() != world)
		{
			continue;
		}

		iter->OnMenuActionTriggered.AddUObject(this, &AMenuGameMode::onMenuAction);
	}
	
	for (TObjectIterator<AMenuCharacter> iter; iter; ++iter)
	{
		iter->OnMenuActionTriggered.AddUObject(this, &AMenuGameMode::onMenuAction);
	}
}

void AMenuGameMode::onMenuAction(TEnumAsByte<EMenuAction::Type> menuAction)
{
	switch (menuAction)
	{
		case EMenuAction::Type::HostGame:
		{
			hostGame();
			break;
		}
		case EMenuAction::Type::JoinGame:
		{
			findGame();
			break;
		}
		case EMenuAction::Type::StartMeleeTutorial:
		{
			startMeleeTutorial();
			break;
		}
		case EMenuAction::Type::StartRangedTutorial:
		{
			startRangedTutorial();
			break;
		}
	}
}

void AMenuGameMode::setTutorialSettings()
{
	UMlgGameInstance* gameInstance = CastChecked<UMlgGameInstance>(GetGameInstance());
	WaveSystemSavedState& wsss = gameInstance->WaveSystemSavedState;
	wsss.currentWaveNumber = 1;
	wsss.remainingEnemies = 0;
	wsss.startWaveNumber = 1;
}

void AMenuGameMode::startRangedTutorial()
{
	setTutorialSettings();
	GetWorld()->ServerTravel(RANGED_TUTORIAL_MAP);
}

void AMenuGameMode::startMeleeTutorial()
{
	setTutorialSettings();
	GetWorld()->ServerTravel(MELEE_TUTORIAL_MAP);
}

void AMenuGameMode::hostGame()
{
	getMlgGameInstance()->HostSession(false, true, 2);
}

void AMenuGameMode::findGame()
{
	UMlgGameInstance* gameInstance = getMlgGameInstance();

	gameInstance->OnFindSessionFinished.AddUObject(this, &AMenuGameMode::onGamesFound);
	ULocalPlayer* localplayer = GetWorld()->GetFirstLocalPlayerFromController();
	gameInstance->FindSessions(localplayer, false);
}

void AMenuGameMode::onGamesFound(const TArray <FOnlineSessionSearchResult>& foundGames)
{
	UMlgGameInstance* gameInstance = getMlgGameInstance();

	gameInstance->OnFindSessionFinished.RemoveAll(this);
	// For now we just immediately join the first found session
	// later we can add a menu, where this can be selected
	if (foundGames.Num() > 0)
	{
		joinGame(foundGames[0]);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("AMenuGameMode::onGamesFound: No game found"));
	}
}

void AMenuGameMode::joinGame(const FOnlineSessionSearchResult& searchResultToJoin)
{
	ULocalPlayer* localplayer = GetWorld()->GetFirstLocalPlayerFromController();
	getMlgGameInstance()->JoinSession(localplayer, searchResultToJoin);
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
