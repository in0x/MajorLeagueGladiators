// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "UsableItem.h"

AUsableItem::AUsableItem()
{
	auto meshComp = GetStaticMeshComponent();

	if (meshComp)
	{
		meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		meshComp->bGenerateOverlapEvents = true;
	}

	SetReplicates(true);
	bReplicateMovement = true;
}

void AUsableItem::Use(AActor* User, TriggerType Type)
{
	UE_LOG(DebugLog, Warning, TEXT("Executing base Use in AUsableItem"));
}

