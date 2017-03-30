// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageVisualizerComponent.h"
#include "DamageReceiverComponent.h"

UDamageVisualizerComponent::UDamageVisualizerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageVisualizerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FDamageVisual& visual : visuals)
	{
		float scaledValue = (visual.MatVisDuration - visual.CurrentMatVisDuration) / visual.MatVisDuration;
		visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, FMath::Clamp(scaledValue, 0.f, 1.f));
		visual.CurrentMatVisDuration += DeltaTime;
	}

	visuals.RemoveAll([](FDamageVisual& visual)
	{
		if (visual.CurrentMatVisDuration >= visual.MatVisDuration)
		{
			visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, 0.0f);
			return true;
		}
		return false;
	});
}

void UDamageVisualizerComponent::AddVisual_NetMulticast_Implementation(UMeshComponent* AffectedMesh, bool bSpawnParticles, const FTransform& ParticleTrafo, const UDamageType* DamageType)
{
	FDamageVisual visual;

	if (bSpawnParticles)
	{
		//TODO switch
		//visual.ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gunDamageParticleSystems[0], ParticleTrafo);
	}

	bool hasVisual = visuals.ContainsByPredicate([&](const FDamageVisual& d)
	{
		return d.AffectedMesh == AffectedMesh;
	});

	if (hasVisual) // If we already have a mesh effect, we disregard the new one and just play particles.
	{
		return;
	}

	visual.MatInstance = AffectedMesh->CreateAndSetMaterialInstanceDynamic(0);
	visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, 1.0f);
	visuals.Add(visual);
}