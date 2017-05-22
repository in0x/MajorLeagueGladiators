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
	virtual void Tick(float DeltaSeconds);
	void OnHitInteractable(const ABaseProjectile* projectile);	
	FTransform GetReflectSpawnTransform() const;
	void PlayReflectSound();
	float GetCurrentActiveTime() { return currentActiveTime; }

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	USoundCue* spawnSoundCue;

	UPROPERTY(EditAnywhere)
	USoundCue* reflectSoundCue;

	void playSpawnSound();

	UPROPERTY(Transient, ReplicatedUsing)
	float maxCurrentActiveTime;

	float currentActiveTime;
};
