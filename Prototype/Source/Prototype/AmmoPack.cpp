// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "Messages/MsgAmmoRefill.h"

AAmmoPack::AAmmoPack()
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

void AAmmoPack::BeginPlay()
{
	Super::BeginPlay();

	msgEndpoint = FMessageEndpoint::Builder("AmmoPackMessager");
	checkf(msgEndpoint.IsValid(), TEXT("Ammo Pack Msg Endpoint invalid"));
}

void AAmmoPack::Use(AActor* User, TriggerType Type)
{
	if (Type == TriggerType::Ammo)
	{
		FMsgAmmoRefill* msg = new FMsgAmmoRefill();
		msg->TriggerActor = User;
		msg->Amount = amountToRefill;
		msgEndpoint->Publish<FMsgAmmoRefill>(msg);

		FVector trashLocation(0, 0, -300);
		TeleportTo(trashLocation, FRotator());
	}
}

