// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MlgGameMode.generated.h"

namespace EMenuAction { enum Type; }

class AMlgPlayerCharacter;
class UWaveSpawnerManagerComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMlgGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	void BeginMatch(int32 StartWave);

	void MatchLost();

	void TravelToRoomOfShame();
	void TravelToGameMap();
	void DestroyAllAi();
private:
	void onMenuAction(TEnumAsByte<EMenuAction::Type> menuAction);

	void filterOutAiPlayerStates();

	void enterGameMap();
	void postEnterRoomOfShame();
	
	// The Tank Class. For now this is always assigned to the client.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> tankClass;

	// The DPS Class. For now this is always assigned to the server.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> dpsClass;

	UPROPERTY(EditAnywhere)
	UWaveSpawnerManagerComponent* waveSpawnerManger;

	UPROPERTY(EditAnywhere)
	int32 easyStartWave;

	UPROPERTY(EditAnywhere)
	int32 mediumStartWave;

	UPROPERTY(EditAnywhere)
	int32 hardStartWave;
};
