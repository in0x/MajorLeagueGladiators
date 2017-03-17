// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


// Sets default values
AParticleSystemManagerActor::AParticleSystemManagerActor()
{
}

void AParticleSystemManagerActor::SpawnParticleSystemAtLocation(UParticleSystem* ParticleSystemTemplate, FTransform Trans, bool AutoDestroy)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans, AutoDestroy);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans.GetLocation(), Trans.GetRotation(), AutoDestroy);
	CreateParticleSystem_NetMulticast(ParticleSystemTemplate, GetWorld(), this, AutoDestroy);
	//UParticleSystemComponent* PSC = CreateParticleSystem_NetMulticast(psTemplate, GetWorld(), this, autoDestroy);
	//particleSystems.Add(PSC);
}

void AParticleSystemManagerActor::CreateParticleSystem_NetMulticast_Implementation(UParticleSystem* EmitterTemplate, UWorld* World, AActor* Actor, bool bAutoDestroy)
{
	UParticleSystemComponent* PSC = NewObject<UParticleSystemComponent>((Actor ? Actor : (UObject*)World));
	PSC->bAutoDestroy = bAutoDestroy;
	PSC->bAllowAnyoneToDestroyMe = true;
	PSC->SecondsBeforeInactive = 0.0f;
	PSC->bAutoActivate = false;
	PSC->SetTemplate(EmitterTemplate);
	PSC->bOverrideLODMethod = false;
}