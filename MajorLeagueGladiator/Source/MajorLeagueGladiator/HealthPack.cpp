// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "Messages/MsgHealthRefill.h"

AHealthPack::AHealthPack()
{
	SetReplicates(true);
	bStaticMeshReplicateMovement = true;
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
	checkf(msgEndpoint.IsValid(), TEXT("Health Pack Msg Endpoint invalid"));
}

void AHealthPack::Use(AActor* User, TriggerType Type)
{
	if (Type == TriggerType::Health)
	{
		FMsgHealthRefill* healthMsg = new FMsgHealthRefill;
		healthMsg->TriggerActor = User;
		healthMsg->Amount = amountToRefill;
		msgEndpoint->Publish<FMsgHealthRefill>(healthMsg);
		
		ReleaseFromGrippedComponent();
		Destroy();
	}
}
