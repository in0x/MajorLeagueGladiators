// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageVisualizerComponent.h"
#include "DamageReceiverComponent.h"

UDamageVisualizerComponent::UDamageVisualizerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UParticleSystem> particles(TEXT("ParticleSystem'/Game/ParticleSystems/PS_BasicDamage.PS_BasicDamage'"));
	particleTemplate = particles.Object;
}

void UDamageVisualizerComponent::BeginPlay()
{
	Super::BeginPlay();

	UMeshComponent* targetMeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();

	if (targetMeshComponent)
	{
		matInstance = targetMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("No MeshComponent found."));
	}

	UDamageReceiverComponent* damageReceiver = GetOwner()->FindComponentByClass<UDamageReceiverComponent>();

	if (damageReceiver)
	{
		damageReceiver->OnDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onDamageReceived);
		damageReceiver->OnPointDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onPointDamageReceived);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("UDamageReceiverComponent missing."));
	}
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

void UDamageVisualizerComponent::AddVisual_NetMulticast_Implementation(UMeshComponent* AffectedMesh, bool bSpawnParticles, const FTransform& ParticleTrafo = FTransform())
{
	FDamageVisual visual;

	if (bSpawnParticles)
	{
		visual.ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleTemplate, ParticleTrafo);
	}

	bool hasVisual = visuals.ContainsByPredicate([&](const FDamageVisual& d)
}
//////////////////////////////
// Damage Receive Functions //
//////////////////////////////

void UDamageVisualizerComponent::onDamageReceived(AActor* DamagedActor, const UDamageType* DamageType)
{

	startMaterialVisualization_NetMulticast();

	//TODO: General Damage PS
}

void UDamageVisualizerComponent::onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation, const FVector& OriginDirection, const UDamageType* DamageType)
{
	FTransform visualizationOrigin;
	visualizationOrigin.SetLocation(HitLocation);
	//The emitter's pitch has to be rotated by 90 to get the expected look at rotation
	FRotator emitterRotator = UKismetMathLibrary::FindLookAtRotation(HitLocation, HitLocation + OriginDirection).Add(-90, 0, 0);
	visualizationOrigin.SetRotation(FQuat(emitterRotator));
	
	startParticleSystemVisualization_NetMulticast(visualizationOrigin);
}

/////////////////////////////
// Visualization Functions //
/////////////////////////////

void UDamageVisualizerComponent::startMaterialVisualization_NetMulticast_Implementation()
{
	if (matInstance)
	{
		return d.AffectedMesh == AffectedMesh;
	});

	if (hasVisual) // If we already have a mesh effect, we disregard the new one and just play particles.
	{
		return;
		UE_LOG(DebugLog, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}

	visual.MatInstance = AffectedMesh->CreateAndSetMaterialInstanceDynamic(0);
	visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, 1.0f);
	visuals.Add(visual);
}

void UDamageVisualizerComponent::stopMaterialVisualization()
{
	isVisualizingMaterial = false;
	currentMatVisualizationDuration = 0.f;
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName(DAMAGE_VALUE_PARAMETER_NAME), 0.0f);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}
