// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AIDamageFeedbackComponent.h"
#include "MlgGameplayStatics.h"
#include "DamageTypes/HitscanProjectileDamage.h"
#include "DamageTypes/SwordDamage.h"

UAIDamageFeedbackComponent::UAIDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//Sounds:
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack01(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_01_16bit_Cue.Sword_Attack_01_16bit_Cue'"));
	swordSounds.Add(swordAttack01.Object);
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack02(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_02_16bit_Cue.Sword_Attack_02_16bit_Cue'"));
	swordSounds.Add(swordAttack02.Object);
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack03(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_03_16bit_Cue.Sword_Attack_03_16bit_Cue'"));
	swordSounds.Add(swordAttack03.Object);
}

void UAIDamageFeedbackComponent::DoParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
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

void UAIDamageFeedbackComponent::DoWeakpointParticleSystemVisualization_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
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
	else if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		for (auto* ps : swordWeakpointDamageParticleSystems)
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

void UAIDamageFeedbackComponent::PlaySound_NetMulticast_Implementation(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	UDamageFeedbackComponent::PlaySound_NetMulticast_Implementation(HitLocation, OriginDirection, DamageType);

	if (DamageType->IsChildOf(USwordDamage::StaticClass()))
	{
		playSwordHitSound(HitLocation);
	}
}

void UAIDamageFeedbackComponent::playSwordHitSound(const FVector& location)
{
	int32 idx = FMath::RandRange(0, swordSounds.Num() - 1);
	FSoundParams soundParams;
	soundParams.Location = location;
	soundParams.Sound = swordSounds[idx];
	checkf(idx >= 0 && GetOwner(), TEXT("Either, index was below zero OR no owner could be found."));
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(GetOwner()->GetWorld(), soundParams);
}