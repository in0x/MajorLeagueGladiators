// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageVisualizerComponent.h"
#include "DamageReceiverComponent.h"

UDamageVisualizerComponent::UDamageVisualizerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageVisualizerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!targetMeshComponent)
	{
		targetMeshComponent = CastChecked<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	}

	if (targetMeshComponent)
	{
		UMaterialInterface* matIface = targetMeshComponent->GetMaterial(0);
		matInstance = UMaterialInstanceDynamic::Create(matIface, targetMeshComponent); //GetOwner()
		targetMeshComponent->SetMaterial(0, matInstance);
	}

	damageReceiver = GetOwner()->FindComponentByClass<UDamageReceiverComponent>();

	if (damageReceiver)
	{
		damageReceiver->OnDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onDamageReceived);
		damageReceiver->OnPointDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onPointDamageReceived);
	}
}

//////////////////////////////
// Damage Receive Functions //
//////////////////////////////

void UDamageVisualizerComponent::onDamageReceived(AActor* DamagedActor)
{
	startMaterialVisualization();

	//Only temporary:
	FTransform trans = GetOwner()->GetTransform();
	trans.AddToTranslation(FVector(0, 50, 0));
	startParticleSystemVisualization(trans);
}

void UDamageVisualizerComponent::onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation)
{
	FTransform visualizationOrigin(HitLocation);

	startParticleSystemVisualization(visualizationOrigin);
}

/////////////////////////////
// Visualization Functions //
/////////////////////////////

void UDamageVisualizerComponent::startMaterialVisualization()
{
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName("DamageValue"), 1.0f);

		FTimerManager& timer = GetOwner()->GetWorldTimerManager();
		FTimerHandle timerHandle;
		timer.SetTimer(timerHandle, this, &UDamageVisualizerComponent::stopMaterialVisualization, materialVisualizationDuration, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}

void UDamageVisualizerComponent::startParticleSystemVisualization(const FTransform& visualizationOrigin)
{

	auto ps = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, visualizationOrigin, true);
	ps->Activate();
	particleSystemInstances.Add(ps);

	FTimerManager& timer = GetOwner()->GetWorldTimerManager();
	FTimerHandle timerHandle;
	timer.SetTimer(timerHandle, this, &UDamageVisualizerComponent::stopParticleSystemVisualization, 2.0f, false);
}

void UDamageVisualizerComponent::stopMaterialVisualization()
{
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName("DamageValue"), 0.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}

void UDamageVisualizerComponent::stopParticleSystemVisualization()
{
	auto ps = particleSystemInstances.Pop();
	ps->Deactivate();
	ps->DestroyComponent();
}