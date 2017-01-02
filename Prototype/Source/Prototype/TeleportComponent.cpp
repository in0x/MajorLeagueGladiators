// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "TeleportComponent.h"

UTeleportComponent::UTeleportComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	arcSpline = ObjectInitializer.CreateDefaultSubobject<USplineMeshComponent>(this, TEXT("ArcSpline"));
	arcSpline->SetMaterial(0, ArcSplineMaterial);
	arcSpline->SetMaterial(1, ArcSplineMaterial);
}

void UTeleportComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetActive(false);
}

void UTeleportComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

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

	arcSpline->SetStartAndEnd(origin->GetComponentLocation(), FVector(0.5f, 0.f, 0.5f), tpHitResult.ImpactPoint, FVector(0.5f, 0.f, 0.5f));

	/*int32 SplinePoints = Spline->GetNumSplinePoints() - 1;
	for (int32 i = 0; i < SplinePoints; ++i) {
		FVector Location, Tangent, LocationNext, TangentNext;
		Spline->GetLocalLocationAndTangentAtSplinePoint(i, Location, Tangent);
		Spline->GetLocalLocationAndTangentAtSplinePoint(i + 1, LocationNext, TangentNext);
		auto *s = ConstructObject<USplineMeshComponent>(USplineMeshComponent::StaticClass(), GetOwner());
		s->SetStaticMesh(Mesh);
		s->SetStartAndEnd(Location, Tangent, LocationNext, TangentNext);
	}*/
}

void UTeleportComponent::Enable(USceneComponent* teleportOrigin)
{
	origin = teleportOrigin;
	Activate();

	arcSpline->Activate();
}

TeleportData UTeleportComponent::GetTeleportData()
{
	if (origin)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Returning Data"));
		return TeleportData{ tpHitResult.ImpactPoint, shouldTeleport };
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Returning Default"));
		return TeleportData{};
	}
}

void UTeleportComponent::Disable()
{
	origin = nullptr;
	Deactivate();
	arcSpline->Deactivate();
}

