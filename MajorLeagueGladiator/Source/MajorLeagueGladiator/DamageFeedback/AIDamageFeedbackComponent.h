// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageFeedbackComponent.h"
#include "AIDamageFeedbackComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UAIDamageFeedbackComponent : public UDamageFeedbackComponent
{
	GENERATED_BODY()
public:

	UAIDamageFeedbackComponent();

	virtual void DoParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr) override;

	virtual void DoWeakpointParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr) override;
	
	virtual void PlaySound(const FVector& location, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr) override;
	
protected:

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	TArray<UParticleSystem*> hitscanDamageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	TArray<UParticleSystem*> swordDamageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	TArray<UParticleSystem*> hitscanWeakpointDamageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	TArray<UParticleSystem*> swordWeakpointDamageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	TArray<USoundBase*> hitscanSounds;

	UPROPERTY(EditAnywhere, Category = "Feedback (AI)")
	USoundCue* swordSoundCue;

	void playSwordHitSound(const FVector& location);
};
