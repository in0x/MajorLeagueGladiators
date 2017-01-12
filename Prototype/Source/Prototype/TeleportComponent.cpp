// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "TeleportComponent.h"

UTeleportComponent::UTeleportComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTeleportComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetActive(false);
}

void UTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!origin)
		return;

	shouldTeleport = UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		tpHitResult,
		positions,
		lastTraceDest,
		origin->GetComponentLocation(),
		origin->GetForwardVector() * 500,
		true,
		0,
		queryTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		1.f
	);
}

void UTeleportComponent::Enable(USceneComponent* TeleportOrigin)
{
	origin = TeleportOrigin;
	Activate();
}

TeleportResult UTeleportComponent::GetTeleportResult() const
{
	if (origin)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Returning Data"));
		return TeleportResult{ tpHitResult.ImpactPoint, shouldTeleport };
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Returning Default"));
		return TeleportResult{};
	}
}

void UTeleportComponent::Disable()
{
	origin = nullptr;
	Deactivate();
}

