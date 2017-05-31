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

namespace
{
	const FString PRE_GAME_MAP("/Game/PreGame");
	const FString GAME_MAP("/Game/ScaleRef");
}

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();
	GameStateClass = AMlgGameState::StaticClass();
	waveSpawnerManger = ObjectInitializer.CreateDefaultSubobject<UWaveSpawnerManagerComponent>(this, TEXT("WaveSpawnerManager"));
//	bUseSeamlessTravel = true;
}

void AMlgGameMode::BeginPlay()
{
	Super::BeginPlay();
	SavedState& savedState = CastChecked<UMlgGameInstance>(GetGameInstance())->savedState;
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->SetFromSavedState(savedState);

	if (savedState.isTravelingToGameMap)
	{
		postEnterGameMap();
	}
	else
	{
		postEnterRoomOfShame();
	}
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

void AMlgGameMode::BeginMatchDefault()
{
	BeginMatch(1);
}

void AMlgGameMode::BeginMatch(int32 StartWave)
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->SetStartWave(StartWave);
	TravelToGameMap();
}

void AMlgGameMode::MatchLost()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->Stop();
	TravelToRoomOfShame();
}

void AMlgGameMode::TravelToRoomOfShame()
{
	SavedState& savedState = CastChecked<UMlgGameInstance>(GetGameInstance())->savedState;
	savedState.isTravelingToGameMap = false;

	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->WriteIntoSavedState(savedState);

	filterOutAiPlayerStates();
	GetWorld()->ServerTravel(PRE_GAME_MAP, true);
}

void AMlgGameMode::TravelToGameMap()
{
	SavedState& savedState = CastChecked<UMlgGameInstance>(GetGameInstance())->savedState;
	savedState.isTravelingToGameMap = true;

	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->WriteIntoSavedState(savedState);

	GetWorld()->ServerTravel(GAME_MAP, true);
}

void AMlgGameMode::filterOutAiPlayerStates()
{
	GameState->PlayerArray.RemoveAllSwap([](APlayerState* playerState)
	{
		return  playerState == nullptr || playerState->bIsABot != 0;
	});
}

void AMlgGameMode::postEnterGameMap()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->StartWave();
}

void AMlgGameMode::postEnterRoomOfShame()
{
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &AMlgGameMode::BeginMatchDefault, 5.f);
}

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}