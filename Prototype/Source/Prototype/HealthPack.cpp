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
		mesh->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlapBegin);
	}
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

	msgEndpoint = FMessageEndpoint::Builder("HealthPackMessager").Build();
}

void AHealthPack::OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit)
{
	auto respone = GetStaticMeshComponent()->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1);
	//UE_LOG(DebugLog, Log, TEXT("%s %d"), *OtherComp->GetFName().ToString(), static_cast<int>(respone));
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("%s %d"), *OtherComp->GetFName().ToString(), static_cast<int>(respone)));
	
	if (Cast<UTriggerZoneComponent>(OtherComp))
	{
		UE_LOG(DebugLog, Log, TEXT("MOTHERFUCKER"));
	}
}

void AHealthPack::IUse(AActor* CollidingActor, TriggerType triggerType)
{
	if (triggerType == TriggerType::Health)
	{
		FMsgHealthRefill* healthMsg = new FMsgHealthRefill;
		healthMsg->TriggerActor = CollidingActor;
		healthMsg->Amount = amountToRefill;
		msgEndpoint->Publish<FMsgHealthRefill>(healthMsg);

		FMsgDropGrip* dropMsg = new FMsgDropGrip;
		dropMsg->ActorToDrop = this;
		msgEndpoint->Publish<FMsgDropGrip>(dropMsg);

		Destroy();
	}
}

void AHealthPack::Use(AActor* CollidingActor, UTriggerZoneComponent* trigger)
{
	/*if (trigger->GetTriggerType() == TriggerType::Health)
	{
		FMsgHealthRefill* healthMsg = new FMsgHealthRefill;
		healthMsg->TriggerActor = CollidingActor;
		healthMsg->Amount = amountToRefill;
		msgEndpoint->Publish<FMsgHealthRefill>(healthMsg);
	
		FMsgDropGrip* dropMsg = new FMsgDropGrip;
		dropMsg->ActorToDrop = this;
		msgEndpoint->Publish<FMsgDropGrip>(dropMsg);

		Destroy();
	}*/
}