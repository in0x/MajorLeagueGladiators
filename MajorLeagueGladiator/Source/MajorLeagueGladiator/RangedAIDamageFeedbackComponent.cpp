// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedAIDamageFeedbackComponent.h"

#include "DamageTypes/HitscanProjectileDamage.h"
#include "DamageTypes/SwordDamage.h"


URangedAIDamageFeedbackComponent::URangedAIDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Ranged.PS_HitscanBurst_Ranged'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Ranged.PS_SwordHit_Ranged'"));
	swordDamageParticleSystems.Add(swordHit_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Ranged.PS_SwordWeakpointHit_Ranged'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Ranged.Object);
	//TODO: add minor PS with sparks....

}

void URangedAIDamageFeedbackComponent::DoParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	UDamageFeedbackComponent::DoParticleSystemVisualization_NetMulticast_Implementation(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(UHitscanProjectileDamage::StaticClass()))
	{
		for (auto* ps : hitscanDamageParticleSystems)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, FTransform(HitLocation));
		}
	}
	//TODO: Sword must deal point damage (rebase to current master)
	else if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		for (auto* ps : swordDamageParticleSystems)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, FTransform(HitLocation));
		}
	}

}

void URangedAIDamageFeedbackComponent::DoWeakpointParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
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
	//TODO: Sword must deal point damage (rebase to current master)
}