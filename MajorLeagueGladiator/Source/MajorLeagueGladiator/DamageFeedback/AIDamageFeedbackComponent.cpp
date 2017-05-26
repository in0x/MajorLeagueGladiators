// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AIDamageFeedbackComponent.h"
#include "MlgGameplayStatics.h"
#include "DamageTypes/HitscanProjectileDamage.h"
#include "DamageTypes/SwordDamage.h"

UAIDamageFeedbackComponent::UAIDamageFeedbackComponent()
{
	//Sounds:
	static ConstructorHelpers::FObjectFinder<USoundCue> swordSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_Cue.Sword_Attack_Cue'"));
	swordSoundCue = swordSoundCueFinder.Object;
}

void UAIDamageFeedbackComponent::DoParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	Super::DoParticleSystemVisualization(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(UHitscanProjectileDamage::StaticClass()))
	{
		for (auto* ps : hitscanDamageParticleSystems)
		{

			UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(),  FEmitterSpawnParams(ps, FTransform(HitLocation)));
		}
	}
	else if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		for (auto* ps : swordDamageParticleSystems)
		{
			FTransform visualizationOrigin;
			visualizationOrigin.SetLocation(HitLocation);
			//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
			FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
			visualizationOrigin.SetRotation(FQuat(emitterRotator));
			UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, visualizationOrigin));
		}
	}

}

void UAIDamageFeedbackComponent::DoWeakpointParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	Super::DoWeakpointParticleSystemVisualization(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(UHitscanProjectileDamage::StaticClass()))
	{
		for (auto* ps : hitscanWeakpointDamageParticleSystems)
		{
			FTransform visualizationOrigin;
			visualizationOrigin.SetLocation(HitLocation);
			//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
			FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
			visualizationOrigin.SetRotation(FQuat(emitterRotator));
			UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, visualizationOrigin));
		}
	}
	else if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		for (auto* ps : swordWeakpointDamageParticleSystems)
		{
			FTransform visualizationOrigin;
			visualizationOrigin.SetLocation(HitLocation);
			//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
			FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
			visualizationOrigin.SetRotation(FQuat(emitterRotator));
			UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, visualizationOrigin));
		}
	}
}

void UAIDamageFeedbackComponent::PlaySound(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	Super::PlaySound(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		playSwordHitSound(HitLocation);
	}
}

void UAIDamageFeedbackComponent::playSwordHitSound(const FVector& location)
{
	FSoundParams soundParams;
	soundParams.Location = location;
	soundParams.Sound = swordSoundCue;
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(GetOwner()->GetWorld(), soundParams);
}