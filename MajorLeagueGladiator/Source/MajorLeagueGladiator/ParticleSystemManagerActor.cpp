#include "MajorLeagueGladiator.h"
#include "ParticleSystemManagerActor.h"


AParticleSystemManagerActor::AParticleSystemManagerActor()
{

}

void AParticleSystemManagerActor::SpawnParticleSystemAtLocation(EParticleSystem particleSystem, FTransform Trans, bool AutoDestroy)
{
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans, AutoDestroy);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, Trans.GetLocation(), Trans.GetRotation(), AutoDestroy);
	CreateParticleSystem_NetMulticast(particleSystems[(int32)particleSystem], GetWorld(), this, Trans, AutoDestroy);
}

void AParticleSystemManagerActor::CreateParticleSystem_NetMulticast_Implementation(UParticleSystem* EmitterTemplate, UWorld* World, AActor* Actor, FTransform Trans, bool bAutoDestroy)
{
	UParticleSystemComponent* PSC = NewObject<UParticleSystemComponent>((Actor ? Actor : (UObject*)World));
	PSC->bAutoDestroy = bAutoDestroy;
	PSC->bAllowAnyoneToDestroyMe = true;
	PSC->SecondsBeforeInactive = 0.0f;
	PSC->bAutoActivate = false;
	PSC->SetTemplate(EmitterTemplate);
	PSC->bOverrideLODMethod = false;

	PSC->bAbsoluteLocation = true;
	PSC->bAbsoluteRotation = true;
	PSC->bAbsoluteScale = true;
	PSC->RelativeLocation = Trans.GetLocation();
	PSC->RelativeRotation = Trans.GetRotation().Rotator();
	PSC->RelativeScale3D = Trans.GetScale3D();

	PSC->RegisterComponentWithWorld(World);

	PSC->ActivateSystem(true);
}