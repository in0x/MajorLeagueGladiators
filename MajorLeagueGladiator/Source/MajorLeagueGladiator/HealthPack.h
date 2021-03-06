// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasePack.h"
#include "HealthPack.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHealthPack : public ABasePack
{
	GENERATED_BODY()
	
public:
	AHealthPack();
	
	UFUNCTION()
	virtual void Use(AActor* User, TriggerType Type) override;

private:
	UPROPERTY(EditAnywhere)
	USoundCue* healSoundCue;

	UPROPERTY(EditAnywhere)
	float amountToRefillUncharged;

	UPROPERTY(EditAnywhere)
	float amountToRefillCharged;

	void playHealSound(AActor* user);
};
