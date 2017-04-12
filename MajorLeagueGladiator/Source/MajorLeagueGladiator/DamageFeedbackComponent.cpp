// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageFeedbackComponent.h"


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

void UDamageFeedbackComponent::AddVisual_NetMulticast_Implementation(UMeshComponent* AffectedMesh, bool bSpawnParticles, const FPointDamageEvent& PointDamageEvent, const UDamageType* DamageType)
{
	FDamageFeedbackData feedback;
	//TODO: UGameplayStatics::PlaySoundAtLocation for concrete FeedbackComponents
	if (bSpawnParticles)
	{
		for (UParticleSystem* ps : particleSystems)
		{
			feedback.ParticleSystems.Push(UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ps, FTransform(PointDamageEvent.HitInfo.ImpactPoint)));
		}
	}

	bool hasVisual = feedbackData.ContainsByPredicate([&](const FDamageFeedbackData& d)
	{
		return d.AffectedMesh == AffectedMesh;
	});

	if (hasVisual) // If we already have a mesh effect, we disregard the new one and just play particles.
	{
		return;
	}

	feedback.MatInstance = AffectedMesh->CreateAndSetMaterialInstanceDynamic(0);
	feedback.MatInstance->SetScalarParameterValue(feedback.DamageValParamName, 1.0f);
	feedbackData.Add(feedback);
}
