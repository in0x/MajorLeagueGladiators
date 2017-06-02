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

namespace
{
	const FString PRE_GAME_MAP("/Game/PreGame");
	const FString GAME_MAP("/Game/ScaleRef");
}

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
	: easyStartWave(1)
	, mediumStartWave(5)
	, hardStartWave(8)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();
	GameStateClass = AMlgGameState::StaticClass();
	waveSpawnerManger = ObjectInitializer.CreateDefaultSubobject<UWaveSpawnerManagerComponent>(this, TEXT("WaveSpawnerManager"));
}

void AMlgGameMode::BeginPlay()
{
	Super::BeginPlay();
	for (TObjectIterator<UMenuActionComponent> iter; iter; ++iter)
	{
		iter->OnMenuActionTriggered.AddUObject(this, &AMlgGameMode::onMenuAction);
	}

	if (CastChecked<UMlgGameInstance>(GetGameInstance())->isInRoomOfShame)
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

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}

void AMlgGameMode::travelToRoomOfShame()
{
	CastChecked<UMlgGameInstance>(GetGameInstance())->isInRoomOfShame = true;

	filterOutAiPlayerStates();
	GetWorld()->ServerTravel(PRE_GAME_MAP, true);
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
	if (CastChecked<UMlgGameInstance>(GetGameInstance())->isInRoomOfShame)
	{
		UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
		waveSystemComponent->SetStartWave(StartWave);
		travelToGameMap();
	}
}

void AMlgGameMode::travelToGameMap()
{
	CastChecked<UMlgGameInstance>(GetGameInstance())->isInRoomOfShame = false;

	GetWorld()->ServerTravel(GAME_MAP, true);
}

