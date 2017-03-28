#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


AParticleSystemManagerActor::AParticleSystemManagerActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AParticleSystemManagerActor::SpawnParticleSystemAtLocation(EParticleSystem particleSystem, FTransform Trans, bool AutoDestroy)
{
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
}