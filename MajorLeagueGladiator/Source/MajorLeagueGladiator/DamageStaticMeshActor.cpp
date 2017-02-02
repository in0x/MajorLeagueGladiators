// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageStaticMeshActor.h"
#include "HealthComponent.h"
#include "DamageReceiverComponent.h"

ADamageStaticMeshActor::ADamageStaticMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAlwaysRelevant = true;
	SetReplicates(true);

	health = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, TEXT("Health"));
	rec = ObjectInitializer.CreateDefaultSubobject<UDamageReceiverComponent>(this, TEXT("Rec"));
	
	health->SetIsReplicated(true);
	rec->SetIsReplicated(true);
}

void ADamageStaticMeshActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADamageStaticMeshActor, health);
	DOREPLIFETIME(ADamageStaticMeshActor, rec);
}



