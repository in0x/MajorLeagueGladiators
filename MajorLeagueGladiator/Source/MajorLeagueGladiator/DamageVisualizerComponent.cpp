// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageVisualizerComponent.h"
#include "DamageReceiverComponent.h"

UDamageVisualizerComponent::UDamageVisualizerComponent()
	: isVisualizingMaterial(false), currentMatVisualizationDuration(0.f)
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
			float val = materialVisualizationDuration - currentMatVisualizationDuration;
			float scaledValue = val / materialVisualizationDuration;
			matInstance->SetScalarParameterValue(FName(DAMAGE_VALUE_PARAMETER_NAME), FMath::Clamp(scaledValue, 0.f, 1.f));

			currentMatVisualizationDuration += DeltaTime;
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
	//trans.AddToTranslation(FVector(0, 0, 0));
	trans.SetRotation(FQuat(FRotator(0.f, -70.f, 0.f)));
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
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, visualizationTransform); //
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
		UE_LOG(LogTemp, Warning, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
}