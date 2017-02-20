// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageVisualizerComponent.h"
#include "DamageReceiverComponent.h"

UDamageVisualizerComponent::UDamageVisualizerComponent()
	: isVisualizingMaterial(false)
{
	PrimaryComponentTick.bCanEverTick = true;
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
		UE_LOG(LogTemp, Warning, TEXT("No MeshComponent found."));
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

void UDamageVisualizerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isVisualizingMaterial)
	{
		if (matInstance)
		{
			matInstance->SetScalarParameterValue(FName(DAMAGE_VALUE_PARAMETER_NAME), 1.0f);
		}
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
		isVisualizingMaterial = true;
		matInstance->SetScalarParameterValue(FName(DAMAGE_VALUE_PARAMETER_NAME), 1.0f);

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
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, visualizationTransform.GetLocation()); //
}

void UDamageVisualizerComponent::stopMaterialVisualization()
{
	isVisualizingMaterial = false;
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName(DAMAGE_VALUE_PARAMETER_NAME), 0.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}