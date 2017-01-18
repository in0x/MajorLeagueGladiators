// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "TriggerZoneComponent.h"

UTriggerZoneComponent::UTriggerZoneComponent()
{
	ToggleVisibility(false);
	bGenerateOverlapEvents = true;
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
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
