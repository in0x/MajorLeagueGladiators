// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PredictedEffectsComponent.h"


// Sets default values for this component's properties
UPredictedEffectsComponent::UPredictedEffectsComponent()
{
	bReplicates = true;
}

bool UPredictedEffectsComponent::IsLocallyControlled() const
{
	const AActor* owner = GetOwner();
	const APawn* ownerPawn = CastChecked<APawn>(owner);
	return ownerPawn->IsLocallyControlled();
}

void UPredictedEffectsComponent::CreateParticleSystemNetworkedPredicted(const FEmitterSpawnParams& Params) const
{
	if (!IsLocallyControlled())
	{
		UE_LOG(DebugLog, Warning, TEXT("CreateParticleSystemNetworkedPredicted called from non local actor. This probably does not make sense"))
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("REMOVE THIS LOG. Predicted effects right use"));
	}

	CreateParticleSystemLocal(Params);
	createParticleSystem_Server(Params);
}

void UPredictedEffectsComponent::CreateParticleSystemLocal(const FEmitterSpawnParams& Params) const
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Params.Template, Params.Transform, Params.bAutoDestroy);
	check(psc);
}
                            
bool UPredictedEffectsComponent::createParticleSystem_Server_Validate(const FEmitterSpawnParams& Params)
{
	return true;
}

void UPredictedEffectsComponent::createParticleSystem_Server_Implementation(const FEmitterSpawnParams& Params) const
{
	createParticleSystem_NetMulticast(Params);
}

void UPredictedEffectsComponent::createParticleSystem_NetMulticast_Implementation(const FEmitterSpawnParams& Params) const
{
	// Prevent Playing Twice
	if (!IsLocallyControlled())
	{
		CreateParticleSystemLocal(Params);;
	}	
}
