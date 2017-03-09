// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameMode.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerState.h"
#include "ParticleSystemManagerActor.h"
#include "MlgGameState.h"

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();


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
			UE_LOG(LogTemp, Warning, TEXT("DPS Class not set. Using Default Pawn Class"));
			return Super::GetDefaultPawnClassForController_Implementation(InController);
		}

		return dpsClass.Get();
	}
	else
	{
		if (tankClass.Get() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tank Class not set. Using Default Pawn Class"));
			return Super::GetDefaultPawnClassForController_Implementation(InController);
		}

		return tankClass.Get();
	}
}

void AMlgGameMode::StartPlay()
{
	Super::StartPlay();

	FVector location(0);
	AParticleSystemManagerActor* psManager = GetWorld()->SpawnActor<AParticleSystemManagerActor>(AParticleSystemManagerActor::StaticClass(), location, FRotator::ZeroRotator);
	CastChecked<AMlgGameState>(GetWorld()->GetGameState())->
}
