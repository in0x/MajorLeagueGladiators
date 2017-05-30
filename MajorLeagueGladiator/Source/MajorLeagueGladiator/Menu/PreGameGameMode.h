// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "PreGameGameMode.generated.h"

class AMlgPlayerCharacter;

UCLASS()
class MAJORLEAGUEGLADIATOR_API APreGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APreGameGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	virtual void BeginPlay() override;

	void StartGame();

private:
	// The Tank Class. For now this is always assigned to the client.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> tankClass;

	// The DPS Class. For now this is always assigned to the server.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> dpsClass;

	FTimerHandle timerHandle;
};
