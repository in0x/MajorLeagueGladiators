// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "CollideStaticMeshActor.h"

ACollideStaticMeshActor::ACollideStaticMeshActor()
{
	OnActorHit.AddDynamic(this, &ACollideStaticMeshActor::OnHit);
}

void ACollideStaticMeshActor::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("HIT"));
}