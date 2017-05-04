// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleeAIDamageFeedbackComponent.h"

#include "DamageTypes/HitscanProjectileDamage.h"
#include "DamageTypes/SwordDamage.h"


UMeleeAIDamageFeedbackComponent::UMeleeAIDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Melee.PS_HitscanBurst_Melee'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Melee.PS_SwordHit_Melee'"));
	swordDamageParticleSystems.Add(swordHit_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Melee.PS_SwordWeakpointHit_Melee'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Melee.Object);
	//TODO: add minor PS with sparks....
	
}

void UMeleeAIDamageFeedbackComponent::DoParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	UDamageFeedbackComponent::DoParticleSystemVisualization_NetMulticast_Implementation(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(UHitscanProjectileDamage::StaticClass()))
	{
		for (auto* ps : hitscanDamageParticleSystems)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, FTransform(HitLocation));
		}
	}
	else if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		for (auto* ps : swordDamageParticleSystems)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, FTransform(HitLocation));
		}
	}

}

void UMeleeAIDamageFeedbackComponent::DoWeakpointParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	UDamageFeedbackComponent::DoWeakpointParticleSystemVisualization_NetMulticast_Implementation(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(UHitscanProjectileDamage::StaticClass()))
	{
		for (auto* ps : hitscanWeakpointDamageParticleSystems)
		{
			FTransform visualizationOrigin;
			visualizationOrigin.SetLocation(HitLocation);
			//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
			FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
			visualizationOrigin.SetRotation(FQuat(emitterRotator));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, visualizationOrigin);
		}
	}
}