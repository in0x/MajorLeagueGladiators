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

	UStaticMeshComponent* targetMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>();

	if (targetMeshComponent)
	{
		UMaterialInterface* matIface = targetMeshComponent->GetMaterial(0); //Currently should only have one.
		matInstance = UMaterialInstanceDynamic::Create(matIface, targetMeshComponent); //GetOwner()
		targetMeshComponent->SetMaterial(0, matInstance);
	}

	UDamageReceiverComponent* damageReceiver = GetOwner()->FindComponentByClass<UDamageReceiverComponent>();

	if (damageReceiver)
	{
		damageReceiver->OnDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onDamageReceived);
		damageReceiver->OnPointDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onPointDamageReceived);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageReceiverComponent missing."));
	}
}

//////////////////////////////
// Damage Receive Functions //
//////////////////////////////

void UDamageVisualizerComponent::onDamageReceived(AActor* DamagedActor)
{

	startMaterialVisualization_NetMulticast();

	//Only temporary: later call only in onPointDamage function
	FTransform trans = GetOwner()->GetTransform();
	trans.AddToTranslation(FVector(0, 50, 0));
	startParticleSystemVisualization_NetMulticast(trans);
}

void UDamageVisualizerComponent::onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation)
{
	FTransform visualizationOrigin(HitLocation);

	startParticleSystemVisualization_NetMulticast(visualizationOrigin);
}

/////////////////////////////
// Visualization Functions //
/////////////////////////////

//Make Net Multicast Function
void UDamageVisualizerComponent::startMaterialVisualization_NetMulticast_Implementation()
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
		UE_LOG(LogTemp, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}

void UDamageVisualizerComponent::startParticleSystemVisualization_NetMulticast_Implementation(const FTransform& visualizationTransform)
{

	auto ps = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, visualizationTransform, true);
	ps->Activate();
	particleSystemInstances.Add(ps);

	FTimerManager& timer = GetOwner()->GetWorldTimerManager();
	FTimerHandle timerHandle;
	timer.SetTimer(timerHandle, this, &UDamageVisualizerComponent::stopParticleSystemVisualization, particleSystemVisualizationDuration, false);
}

void UDamageVisualizerComponent::stopMaterialVisualization()
{
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName("DamageValue"), 0.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}

void UDamageVisualizerComponent::stopParticleSystemVisualization()
{
	auto ps = particleSystemInstances.Pop();
	ps->Deactivate();
	ps->DestroyComponent();
}