#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


AParticleSystemManagerActor::AParticleSystemManagerActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

/************************************************************************/
/*                Particles                                             */
/************************************************************************/

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

/************************************************************************/
/*                Sound                                                 */
/************************************************************************/

void AParticleSystemManagerActor::PlaySoundAtLocation(const FSoundParams& Params) const
{
	playSoundAtLocation_Server(Params);
}

void AParticleSystemManagerActor::playSoundAtLocation_Server_Implementation(const FSoundParams& Params) const
{
	playSoundAtLocation_NetMulticast(Params);
}

bool AParticleSystemManagerActor::playSoundAtLocation_Server_Validate(const FSoundParams& Params)
{
	return true;
}

void AParticleSystemManagerActor::playSoundAtLocation_NetMulticast_Implementation(const FSoundParams& Params) const
{
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		Params.Sound,
		Params.Location,
		Params.Rotation,
		Params.VolumeMultiplier,
		Params.PitchMultiplier,
		Params.StartTime,
		Params.AttenuationSettings,
		Params.ConcurrencySettings
		);
}