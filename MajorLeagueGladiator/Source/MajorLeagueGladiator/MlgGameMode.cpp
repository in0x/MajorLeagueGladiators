// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameMode.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerState.h"
#include "EffectsManagerActor.h"
#include "MlgGameState.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"
#include "Ai/MlgAICharacter.h"

namespace
{
	const FString PRE_GAME_MAP("/Game/PreGame?game=/Script/MajorLeagueGladiator.PreGameGameMode");
}

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();
	GameStateClass = AMlgGameState::StaticClass();
	fxManagerClass = AEffectsManagerActor::StaticClass();
	waveSpawnerManger = ObjectInitializer.CreateDefaultSubobject<UWaveSpawnerManagerComponent>(this, TEXT("WaveSpawnerManager"));
	bUseSeamlessTravel = true;
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

void AMlgGameMode::InitGameState()
{
	Super::InitGameState();

	AEffectsManagerActor* fxManager = GetWorld()->SpawnActor<AEffectsManagerActor>(fxManagerClass.Get(), FVector(0), FRotator::ZeroRotator);

	AMlgGameState* gameState = GetWorld()->GetGameState<AMlgGameState>();
	gameState->SetEffectsManager(fxManager);
}

void AMlgGameMode::TravelToPreGameMap()
{
	DestroyAllAi();
	GetWorld()->ServerTravel(PRE_GAME_MAP, true);
}

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}
