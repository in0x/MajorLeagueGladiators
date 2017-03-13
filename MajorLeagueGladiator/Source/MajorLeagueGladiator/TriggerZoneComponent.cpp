// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TriggerZoneComponent.h"
#include "Usable.h"

namespace
{
	const char* TRIGGER_ZONE_COLLISION_PROFILE_NAME = "TriggerZone";
}

UTriggerZoneComponent::UTriggerZoneComponent()
{
	ToggleVisibility(false);
	bGenerateOverlapEvents = true;
	bMultiBodyOverlap = true;
	SetCollisionProfileName(TRIGGER_ZONE_COLLISION_PROFILE_NAME);
	OnComponentBeginOverlap.AddDynamic(this, &UTriggerZoneComponent::OnOverlapBegin);
}

void UTriggerZoneComponent::BeginPlay()
{
	Super::BeginPlay();
	auto mesh = GetStaticMesh();
	checkf(mesh, TEXT("TriggerZoneComponent requires a mesh")); 
}

TriggerType UTriggerZoneComponent::GetTriggerType() const
{
	return triggerType;
}

void UTriggerZoneComponent::SetTriggerType(TriggerType type)
{
	triggerType = type;
}
 
void UTriggerZoneComponent::OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	auto otherAsUsable = Cast<IUsable>(Other);

	if (otherAsUsable)
	{
		otherAsUsable->Use(GetOwner(), GetTriggerType());
	}
}

