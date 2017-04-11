#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


AParticleSystemManagerActor::AParticleSystemManagerActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AParticleSystemManagerActor::CreateParticleSystemAtLocation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_Server(Params);
}

void AParticleSystemManagerActor::createParticleSystem_Server_Implementation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_NetMulticast(Params);
}

bool AParticleSystemManagerActor::createParticleSystem_Server_Validate(const FEmitterSpawnParams& Params)
{
	return true;
}

void AParticleSystemManagerActor::createParticleSystem_NetMulticast_Implementation(const FEmitterSpawnParams& Params) const
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Params.Template, Params.Transform, Params.bAutoDestroy);
}