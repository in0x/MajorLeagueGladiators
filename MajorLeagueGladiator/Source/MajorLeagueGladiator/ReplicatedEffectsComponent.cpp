#include "MajorLeagueGladiator.h"
#include "ReplicatedEffectsComponent.h"


UReplicatedEffectsComponent::UReplicatedEffectsComponent()
{
	bReplicates = true;
}

/************************************************************************/
/*                Particles                                             */
/************************************************************************/

void UReplicatedEffectsComponent::CreateParticleSystemAtLocation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_Server(Params);
}

void UReplicatedEffectsComponent::createParticleSystem_Server_Implementation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_NetMulticast(Params);
}

bool UReplicatedEffectsComponent::createParticleSystem_Server_Validate(const FEmitterSpawnParams& Params)
{
	return true;
}

void UReplicatedEffectsComponent::createParticleSystem_NetMulticast_Implementation(const FEmitterSpawnParams& Params) const
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Params.Template, Params.Transform, Params.bAutoDestroy);
}

/************************************************************************/
/*                Sound                                                 */
/************************************************************************/

void UReplicatedEffectsComponent::PlaySoundAtLocation(const FSoundParams& Params) const
{
	playSoundAtLocation_Server(Params);
}

void UReplicatedEffectsComponent::playSoundAtLocation_Server_Implementation(const FSoundParams& Params) const
{
	playSoundAtLocation_NetMulticast(Params);
}

bool UReplicatedEffectsComponent::playSoundAtLocation_Server_Validate(const FSoundParams& Params)
{
	return true;
}

void UReplicatedEffectsComponent::playSoundAtLocation_NetMulticast_Implementation(const FSoundParams& Params) const
{
	if (Params.Location.IsZero())
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			Params.Sound,
			Params.VolumeMultiplier,
			Params.PitchMultiplier,
			Params.StartTime,
			Params.ConcurrencySettings
			);
	}
	else
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
}

