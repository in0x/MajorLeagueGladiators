// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "UsableItem.h"
#include "TriggerZoneComponent.h"

AUsableItem::AUsableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

void AUsableItem::BeginPlay()
{
	Super::BeginPlay();
	GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &AUsableItem::OnOverlapBegin);

	auto mesh = GetStaticMeshComponent()->GetStaticMesh();
	checkf(mesh, TEXT("UsableItem requires a mesh"));
}

void AUsableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUsableItem::Use(AActor* ColidingActor, UTriggerZoneComponent* trigger)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Used item"));
}

void AUsableItem::OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	auto trigger = Cast<UTriggerZoneComponent>(OtherComp);

	if (trigger)
	{
		Use(Other, trigger);
	}
}

