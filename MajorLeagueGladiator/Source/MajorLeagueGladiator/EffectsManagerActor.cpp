#include "MajorLeagueGladiator.h"
#include "EffectsManagerActor.h"


AEffectsManagerActor::AEffectsManagerActor()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

/************************************************************************/
/*                Particles                                             */
/************************************************************************/

void AEffectsManagerActor::CreateParticleSystemAtLocation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_Server(Params);
}

void AEffectsManagerActor::createParticleSystem_Server_Implementation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_NetMulticast(Params);
}

bool AEffectsManagerActor::createParticleSystem_Server_Validate(const FEmitterSpawnParams& Params)
{
	return true;
}

void AEffectsManagerActor::createParticleSystem_NetMulticast_Implementation(const FEmitterSpawnParams& Params) const
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Params.Template, Params.Transform, Params.bAutoDestroy);
}

/************************************************************************/
/*                Sound                                                 */
/************************************************************************/

void AEffectsManagerActor::PlaySoundAtLocation(const FSoundParams& Params) const
{
	playSoundAtLocation_Server(Params);
}

void AEffectsManagerActor::playSoundAtLocation_Server_Implementation(const FSoundParams& Params) const
{
	playSoundAtLocation_NetMulticast(Params);
}

bool AEffectsManagerActor::playSoundAtLocation_Server_Validate(const FSoundParams& Params)
{
	return true;
}

void AEffectsManagerActor::playSoundAtLocation_NetMulticast_Implementation(const FSoundParams& Params) const
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