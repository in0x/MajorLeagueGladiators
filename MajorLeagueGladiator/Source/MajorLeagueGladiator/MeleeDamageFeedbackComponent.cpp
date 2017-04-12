// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleeDamageFeedbackComponent.h"


UMeleeDamageFeedbackComponent::UMeleeDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> smokePS(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	particleSystems.Add(smokePS.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> burstPS(TEXT("ParticleSystem'/Game/ParticleSystems/PS_MeleeHitscanBurst.PS_MeleeHitscanBurst'"));
	weakpointParticleSystem = burstPS.Object;
	//TODO: add minor PS with sparks....
}
//
//void UMeleeDamageFeedbackComponent::AddVisual_NetMulticast(UMeshComponent* affectedMesh, bool bSpawnParticles, const FPointDamageEvent& PointDamageEvent, const UDamageType* DamageType)
//{
//	//UDamageFeedbackComponent::AddVisual_NetMulticast(affectedMesh, bSpawnParticles, PointDamageEvent, DamageType);
//
//	FTransform visualizationOrigin;
//	visualizationOrigin.SetLocation(PointDamageEvent.HitInfo.ImpactPoint);
//	//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
//	FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(PointDamageEvent.HitInfo.ImpactPoint, PointDamageEvent.HitInfo.ImpactPoint + PointDamageEvent.ShotDirection).Add(-90, 0, 0);
//	visualizationOrigin.SetRotation(FQuat(emitterRotator));
//}