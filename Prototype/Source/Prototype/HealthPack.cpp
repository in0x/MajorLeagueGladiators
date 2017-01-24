// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgHealthRefill.h"
#include "Messages/MsgDropGrip.h"

AHealthPack::AHealthPack()
{
	SetReplicates(true);

	auto mesh = GetStaticMeshComponent();

	if (mesh)
	{
		mesh->bGenerateOverlapEvents = true;
		mesh->bMultiBodyOverlap = true;
	}
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();
	msgEndpoint = FMessageEndpoint::Builder("HealthPackMessager").Build();
}

void AHealthPack::Use(AActor* User, TriggerType Type)
{
	if (Type == TriggerType::Health)
	{
		FMsgHealthRefill* healthMsg = new FMsgHealthRefill;
		healthMsg->TriggerActor = User;
		healthMsg->Amount = amountToRefill;
		msgEndpoint->Publish<FMsgHealthRefill>(healthMsg);

		FMsgDropGrip* dropMsg = new FMsgDropGrip;
		dropMsg->ActorToDrop = this;
		msgEndpoint->Publish<FMsgDropGrip>(dropMsg);

		Destroy();
	}
}
