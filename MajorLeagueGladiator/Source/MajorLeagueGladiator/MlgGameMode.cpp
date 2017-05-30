// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameMode.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerState.h"
#include "EffectsManagerActor.h"
#include "MlgGameState.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"
#include "Ai/MlgAICharacter.h"
#include "MlgPlayerController.h"

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
	GameState->PlayerArray.RemoveAllSwap([](APlayerState* playerState)
	{
		return  playerState == nullptr || playerState->bIsABot != 0;
	});

	GetWorld()->ServerTravel(PRE_GAME_MAP, true);
}

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}

void AMlgGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	/*Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	ActorList.RemoveAllSwap([](AActor* actor)
	{
		if (APlayerState* ps = Cast<APlayerState>(actor))
		{
			return ps->bIsABot != 0;
		}
		return false;
	});*/
	// Get allocations for the elements we're going to add handled in one go
	const int32 ActorsToAddCount = 2 + (bToTransition ? 3 : 0);
	ActorList.Reserve(ActorsToAddCount);

	for (TActorIterator<AMlgPlayerController> iter(GetWorld(), AMlgPlayerController::StaticClass()); iter; ++iter)
	{
		ActorList.Add(iter->PlayerState);
	}

	if (bToTransition)
	{
		// Keep ourselves until we transition to the final destination
		ActorList.Add(this);
		// Keep general game state until we transition to the final destination
		ActorList.Add(GameState);
		// Keep the game session state until we transition to the final destination
		ActorList.Add(GameSession);

		// If adding in this section best to increase the literal above for the ActorsToAddCount
	}
}
