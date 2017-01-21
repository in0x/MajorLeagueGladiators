// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "TriggerZoneComponent.h"
#include "Usable.h"

UTriggerZoneComponent::UTriggerZoneComponent()
{
	ToggleVisibility(false);
	bGenerateOverlapEvents = true;
	bMultiBodyOverlap = true;
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	OnComponentBeginOverlap.AddDynamic(this, &UTriggerZoneComponent::OnOverlapBegin);
}

void UTriggerZoneComponent::BeginPlay()
{
	auto mesh = GetStaticMesh();
	checkf(mesh, TEXT("TriggerZoneComponent requires a mesh")); 
}

TriggerType UTriggerZoneComponent::GetTriggerType()
{
	return triggerType;
}

void UTriggerZoneComponent::SetTriggerType(TriggerType type)
{
	triggerType = type;
}
 
void UTriggerZoneComponent::OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	// Check for interface here
	UE_LOG(DebugLog, Log, TEXT("UTriggerZoneComponent::OnOverlapBegin() %s"), *OtherComp->GetFName().ToString());

	auto otherAsUsable = Cast<IUsable>(Other);

	if (otherAsUsable)
	{
		UE_LOG(DebugLog, Log, TEXT("TriggerZone User"));

		otherAsUsable->IUse(GetOwner(), GetTriggerType());
	}
}

