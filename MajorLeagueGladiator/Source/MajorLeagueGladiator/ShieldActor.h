// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableMeshActor.h"
#include "ShieldActor.generated.h"

DECLARE_DELEGATE(TimeRunOutDelegate);

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
	float GetCurrentActiveTime() const { return currentActiveTime; }
	float CalcTimeLeft() const;
	float CalcSecondsUntilRecharged() const;
	
	UPROPERTY(Transient, Replicated)
	float startActiveTime;

	TimeRunOutDelegate OnTimeRunOut;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void updateAnimation(float timeRemaining);

	UPROPERTY(EditAnywhere)
	USoundCue* spawnSoundCue;

	UPROPERTY(EditAnywhere)
	USoundCue* reflectSoundCue;

	void playSpawnSound();

	UPROPERTY(EditAnywhere)
	float maxShieldSeconds;

	UPROPERTY(EditAnywhere)
	float centerMaxScale;

	UPROPERTY(EditAnywhere)
	float centerMinScale;

	float currentActiveTime;
};
