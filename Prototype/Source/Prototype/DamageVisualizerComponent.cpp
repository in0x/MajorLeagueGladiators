// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageVisualizerComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgDamageReceived.h"


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
		matInstance = Cast<UMaterialInstanceDynamic>(matIface);
	}

	msgEndpoint = FMessageEndpoint::Builder("DamageReceiverMessager").Handling<FMsgDamageReceived>(this, &UDamageVisualizerComponent::onDamageReceived);

	checkf(msgEndpoint.IsValid(), TEXT("Damage Visualizer Component Msg Endpoint invalid"));
	msgEndpoint->Subscribe<FMsgDamageReceived>();
}

void UDamageVisualizerComponent::onDamageReceived(const FMsgDamageReceived& Msg, const IMessageContextRef& Context)
{
	if (Msg.DamagedActor == GetOwner())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent got FMsgDamageReceived."));

		startDamageVisualization();
	}
}

void UDamageVisualizerComponent::startDamageVisualization()
{
	if (matInstance)
	{
		matInstance->SetScalarParameterValue(FName("DoDamage"), 1.0f);

		FTimerManager& timer = GetOwner()->GetWorldTimerManager();
		timer.SetTimer(spawnTimerHandle, this, &UDamageVisualizerComponent::stopDamageVisualization, 1.0f, false);
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
		matInstance->SetScalarParameterValue(FName("DoDamage"), 0.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent::startDamageVisualization: NO Material Instance!"));
	}
	
}