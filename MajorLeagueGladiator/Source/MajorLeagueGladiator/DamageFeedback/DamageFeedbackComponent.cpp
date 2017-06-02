// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageFeedbackComponent.h"
#include "MlgGameplayStatics.h"


UDamageFeedbackComponent::UDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FDamageFeedbackData& feedback : feedbackData)
	{
		float scaledValue = (feedback.MatVisDuration - feedback.CurrentMatVisDuration) / feedback.MatVisDuration;
		feedback.MatInstance->SetScalarParameterValue(feedback.DamageValParamName, FMath::Clamp(scaledValue, 0.f, 1.f));
		feedback.CurrentMatVisDuration += DeltaTime;
	}

	feedbackData.RemoveAll([](FDamageFeedbackData& feedback)
	{
		if (feedback.CurrentMatVisDuration >= feedback.MatVisDuration)
		{
			feedback.MatInstance->SetScalarParameterValue(feedback.DamageValParamName, 0.0f);
			return true;
		}
		return false;
	});
}

void UDamageFeedbackComponent::DoMaterialVisualization(UMeshComponent* AffectedMesh)
{
	FDamageFeedbackData feedback;
	bool hasVisual = feedbackData.ContainsByPredicate([&](const FDamageFeedbackData& d)
	{
		return d.AffectedMesh == AffectedMesh;
	});
	if (hasVisual)
	{
		return;
	}
	feedback.MatInstance = AffectedMesh->CreateAndSetMaterialInstanceDynamic(0);
	feedback.MatInstance->SetScalarParameterValue(feedback.DamageValParamName, 1.0f);
	feedbackData.Add(feedback);
}

void UDamageFeedbackComponent::DoParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	for (UParticleSystem* ps : damageParticleSystems)
	{
		UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, FTransform(HitLocation)));
	}
}

void UDamageFeedbackComponent::DoWeakpointParticleSystemVisualization(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	
	for (UParticleSystem* ps : weakpointParticleSystems)
	{
		FTransform visualizationOrigin;
		visualizationOrigin.SetLocation(HitLocation);
		//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
		FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
		visualizationOrigin.SetRotation(FQuat(emitterRotator));
		UMlgGameplayStatics::SpawnEmitterNetworked(GetWorld(), FEmitterSpawnParams(ps, visualizationOrigin));
	}
}

void UDamageFeedbackComponent::PlaySound(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType)
{
	for (USoundBase* sound : sounds)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, HitLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr);
		//UMlgGameplayStatics::playSoundNetworked(GetWorld(), FSoundParams(sound, HitLocation));
	}
}