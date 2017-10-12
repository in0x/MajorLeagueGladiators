// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameMode.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerState.h"
#include "MlgGameState.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"
#include "WaveSystem/WaveSystemComponent.h"
#include "Ai/MlgAICharacter.h"
#include "MlgPlayerController.h"
#include "MlgGameInstance.h"
#include "Menu/MenuActionComponent.h"
#include "MlgGameSession.h"

namespace
{
	const FString PRE_GAME_MAP("/Game/PreGame?listen");
	const FString GAME_MAP("/Game/ScaleRef?listen");
}

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
	: easyStartWave(1)
	, mediumStartWave(11)
	, hardStartWave(21)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();
	GameStateClass = AMlgGameState::StaticClass();
	waveSpawnerManger = ObjectInitializer.CreateDefaultSubobject<UWaveSpawnerManagerComponent>(this, TEXT("WaveSpawnerManager"));
	bUseSeamlessTravel = true;
}

TSubclassOf<AGameSession> AMlgGameMode::GetGameSessionClass() const
{
	return AMlgGameSession::StaticClass();
}

void AMlgGameMode::BeginPlay()
{
	Super::BeginPlay();
	for (TObjectIterator<UMenuActionComponent> iter; iter; ++iter)
	{
		iter->OnMenuActionTriggered.AddUObject(this, &AMlgGameMode::onMenuAction);
	}

	if (isInRoomOfShame())
	{
		postEnterRoomOfShame();
	}
	else
	{
		enterGameMap();
	}
}

void AMlgGameMode::enterGameMap()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->StartWave();
}

void AMlgGameMode::postEnterRoomOfShame()
{
}

bool AMlgGameMode::isInRoomOfShame() const
{
	return CastChecked<UMlgGameInstance>(GetGameInstance())->bIsInRoomOfShame;
}

void AMlgGameMode::setIsInRoomOfShame(bool NewIsInRoomOfShame)
{
	CastChecked<UMlgGameInstance>(GetGameInstance())->bIsInRoomOfShame = NewIsInRoomOfShame;
}

UClass* AMlgGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// Handle only Player Controllers for now, as AI is spawned differently
	check(InController->IsPlayerController());

	// For now this just assigns the Server the DPS class and the Client the Tank Class.
	// Game Mode is only on the Server. If the controller is locally controlled it is the
	// server's controller
	if (InController->IsLocalPlayerController())
	{
		if (dpsClass.Get() == nullptr)
		{
			UE_LOG(DebugLog, Warning, TEXT("DPS Class not set. Using Default Pawn Class"));
			return Super::GetDefaultPawnClassForController_Implementation(InController);
		}
		return dpsClass.Get();
	}
	else
	{
		if (tankClass.Get() == nullptr)
		{
			UE_LOG(DebugLog, Warning, TEXT("Tank Class not set. Using Default Pawn Class"));
			return Super::GetDefaultPawnClassForController_Implementation(InController);
		}

		return tankClass.Get();
	}
}

void AMlgGameMode::MatchLost()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->Stop();
	travelToRoomOfShame();
}

void AMlgGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AMlgGameMode::travelToRoomOfShame()
{
	setIsInRoomOfShame(true);

	filterOutAiPlayerStates();
	GetWorld()->ServerTravel(PRE_GAME_MAP, true);
}

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}



void AMlgGameMode::filterOutAiPlayerStates()
{
	GameState->PlayerArray.RemoveAllSwap([](APlayerState* playerState)
	{
		return  playerState == nullptr || playerState->bIsABot != 0;
	});
}

void AMlgGameMode::onMenuAction(TEnumAsByte<EMenuAction::Type> menuAction)
{
	switch (menuAction)
	{
	case EMenuAction::GoToMainMenu:
	{
		travelToMainMenu();
		break;
	}
	case EMenuAction::StartGameEasy:
	{
		beginMatch(easyStartWave);
		break;
	}
	case EMenuAction::StartGameMedium:
	{
		beginMatch(mediumStartWave);
		break;
	}
	case EMenuAction::StartGameHard:
	{
		beginMatch(hardStartWave);
		break;
	}
	default:
	{
		checkNoEntry();
		break;
	}
	}
}

void AMlgGameMode::travelToMainMenu()
{
	CastChecked<UMlgGameInstance>(GetGameInstance())->TravelToMainMenu();
}

void AMlgGameMode::beginMatch(int32 StartWave)
{
	if (isInRoomOfShame())
	{
		UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
		waveSystemComponent->SetStartWave(StartWave);
		travelToGameMap();
	}
}

void AMlgGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AMlgGameMode::travelToGameMap()
{
	setIsInRoomOfShame(false);

	GetWorld()->ServerTravel(GAME_MAP, true);
}

