#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


AParticleSystemManagerActor::AParticleSystemManagerActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AParticleSystemManagerActor::SpawnParticleSystemAtLocation(EParticleSystem particleSystem, FTransform Trans, bool AutoDestroy)
{
	//TODO: Sandro
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans, AutoDestroy);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans.GetLocation(), Trans.GetRotation(), AutoDestroy);
	//CreateParticleSystem_NetMulticast(particleSystems[(int32)particleSystem], GetWorld(), this, Trans, AutoDestroy);
}

void AParticleSystemManagerActor::CreateParticleSystemMain(UParticleSystem* EmitterTemplate, FTransform Trans, bool bAutoDestroy)
{
	CreateParticleSystem_Server(EmitterTemplate, Trans, bAutoDestroy);
}

void AParticleSystemManagerActor::CreateParticleSystem_Server_Implementation(UParticleSystem* EmitterTemplate, FTransform Trans, bool bAutoDestroy)
{
	CreateParticleSystem_NetMulticast(EmitterTemplate, Trans, bAutoDestroy);
}

bool AParticleSystemManagerActor::CreateParticleSystem_Server_Validate(UParticleSystem* EmitterTemplate, FTransform Trans, bool bAutoDestroy)
{
	return true;
}

void AParticleSystemManagerActor::CreateParticleSystem_NetMulticast_Implementation(UParticleSystem* EmitterTemplate, FTransform Trans, bool bAutoDestroy)
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, Trans);
	//psc->SetIsReplicated(true);

	//TODO: Sandro

	//UParticleSystemComponent* PSC = NewObject<UParticleSystemComponent>(this);
	//PSC->bAutoDestroy = bAutoDestroy;
	//PSC->bAllowAnyoneToDestroyMe = true;
	//PSC->SecondsBeforeInactive = 0.0f;
	//PSC->bAutoActivate = false;
	//PSC->SetTemplate(ps);
	//PSC->bOverrideLODMethod = false;

	//PSC->bAbsoluteLocation = true;
	//PSC->bAbsoluteRotation = true;
	//PSC->bAbsoluteScale = true;
	//PSC->RelativeLocation = Trans.GetLocation();
	//PSC->RelativeRotation = Trans.GetRotation().Rotator();
	//PSC->RelativeScale3D = Trans.GetScale3D();
	//PSC->SetIsReplicated(true);

	//PSC->RegisterComponent();
	////PSC->RegisterComponentWithWorld(GetWorld());

	//PSC->ActivateSystem(true);
}