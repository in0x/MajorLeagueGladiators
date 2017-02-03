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

	if (!meshComponent)
	{
		meshComponent = CastChecked<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	}

	if (meshComponent)
	{
		UMaterialInterface* matIface = meshComponent->GetMaterial(0);
		matInstance = UMaterialInstanceDynamic::Create(matIface, meshComponent); //meshComponent/GetOwner()
		meshComponent->SetMaterial(0, matInstance);
		//matInstance = Cast<UMaterialInstanceDynamic>(meshComponent->GetMaterial(0));
	}

	damageReceiver = GetOwner()->FindComponentByClass<UDamageReceiverComponent>();

	if (damageReceiver)
	{
		damageReceiver->OnDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onDamageReceived);
		damageReceiver->OnPointDamageReceived.AddUObject(this, &UDamageVisualizerComponent::onPointDamageReceived);
	}
}

void UDamageVisualizerComponent::onDamageReceived(AActor* DamagedActor)
{
	FTransform visualizationOrigin;

	startDamageVisualization(visualizationOrigin);
}

void UDamageVisualizerComponent::onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation)
{
	FTransform visualizationOrigin(HitLocation);

	startDamageVisualization(visualizationOrigin);
}

void UDamageVisualizerComponent::startDamageVisualization(const FTransform& visualizationOrigin)
{
	if (matInstance)
	{
		auto ps = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, GetOwner()->GetTransform(), false); //visualizationOrigin
		particleSystemInstances.Add(ps);

		matInstance->SetScalarParameterValue(FName("Param1"), 1.0f);

		FTimerManager& timer = GetOwner()->GetWorldTimerManager();
		FTimerHandle timerHandle;
		timer.SetTimer(timerHandle, this, &UDamageVisualizerComponent::stopDamageVisualization, 2.0f, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
	
}

void UDamageVisualizerComponent::stopDamageVisualization()
{
	if (matInstance)
	{
		auto ps = particleSystemInstances.Pop();
		ps->Deactivate();
		ps->DestroyComponent();

		matInstance->SetScalarParameterValue(FName("Param1"), 0.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
	
}