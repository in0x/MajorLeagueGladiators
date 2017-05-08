// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AIDamageFeedbackComponent.h"
#include "MlgGameplayStatics.h"
#include "DamageTypes/HitscanProjectileDamage.h"
#include "DamageTypes/SwordDamage.h"

UAIDamageFeedbackComponent::UAIDamageFeedbackComponent()
{
	//Sounds:
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack01(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_01_16bit_Cue.Sword_Attack_01_16bit_Cue'"));
	swordSounds.Add(swordAttack01.Object);
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack02(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_02_16bit_Cue.Sword_Attack_02_16bit_Cue'"));
	swordSounds.Add(swordAttack02.Object);
	static ConstructorHelpers::FObjectFinder<USoundCue> swordAttack03(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_03_16bit_Cue.Sword_Attack_03_16bit_Cue'"));
	swordSounds.Add(swordAttack03.Object);
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
			UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, FTransform(HitLocation)));
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
	int32 idx = FMath::RandRange(0, swordSounds.Num() - 1);
	FSoundParams soundParams;
	soundParams.Location = location;
	soundParams.Sound = swordSounds[idx];
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(GetOwner()->GetWorld(), soundParams);
}