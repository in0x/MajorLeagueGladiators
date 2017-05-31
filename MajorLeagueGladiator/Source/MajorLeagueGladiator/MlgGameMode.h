// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MlgGameMode.generated.h"

class AMlgPlayerCharacter;
class UWaveSpawnerManagerComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMlgGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	void TravelToPreGameMap();

	void DestroyAllAi();
private:
	void filterOutAiPlayerStates();
	
	// The Tank Class. For now this is always assigned to the client.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> tankClass;

	// The DPS Class. For now this is always assigned to the server.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> dpsClass;

	UPROPERTY(EditAnywhere)
	UWaveSpawnerManagerComponent* waveSpawnerManger;
};
