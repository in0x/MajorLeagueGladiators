// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RespawnComponent.h"
#include "Characters/MlgPlayerCharacter.h"

URespawnComponent::URespawnComponent()
{
	bGenerateOverlapEvents = true;
	bMultiBodyOverlap = true;
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	OnComponentBeginOverlap.AddDynamic(this, &URespawnComponent::OnOverlapBegin);
}

void URespawnComponent::BeginPlay()
{
	Super::BeginPlay();
	auto mesh = GetStaticMesh();
	checkf(mesh, TEXT("RespawnComponent requires a mesh"));
}

void URespawnComponent::OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	if (Other->IsA<AMlgPlayerCharacter>()) 
	{
		Other->TeleportTo(respawnPosition, Other->GetActorRotation());
	}
}

