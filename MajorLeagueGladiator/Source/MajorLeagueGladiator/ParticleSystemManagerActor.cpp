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
	int index = particleSystems.Find(EmitterTemplate);
	if (index >= 0)
	{
		CreateParticleSystem_Server(index, Trans, bAutoDestroy);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No particle system available!"));
	}
	
}

void AParticleSystemManagerActor::CreateParticleSystem_Server_Implementation(int Index, FTransform Trans, bool bAutoDestroy)
{
	CreateParticleSystem_NetMulticast(Index, Trans, bAutoDestroy);
}

bool AParticleSystemManagerActor::CreateParticleSystem_Server_Validate(int Index, FTransform Trans, bool bAutoDestroy)
{
	return true;
}

void AParticleSystemManagerActor::CreateParticleSystem_NetMulticast_Implementation(int Index, FTransform Trans, bool bAutoDestroy)
{
	UParticleSystemComponent* psc = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystems[Index], Trans);
	//psc->SetIsReplicated(true);

	//TODO: Sandro
	//UParticleSystem* ps = particleSystems[Index];
	//if (!ps)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Particle system is null!"));
	//}

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