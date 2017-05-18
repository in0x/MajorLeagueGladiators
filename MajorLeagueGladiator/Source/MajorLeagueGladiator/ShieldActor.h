// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableMeshActor.h"
#include "ShieldActor.generated.h"

class ABaseProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AShieldActor : public AMlgGrippableMeshActor
{
	GENERATED_BODY()
public:
	AShieldActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	void OnHitInteractable(const ABaseProjectile* projectile);	
	FTransform GetReflectSpawnTransform() const;
	void PlayReflectSound();

private:
	UPROPERTY(EditAnywhere)
	USoundCue* spawnSoundCue;

	UPROPERTY(EditAnywhere)
	USoundCue* reflectSoundCue;

	void playSpawnSound();
};
