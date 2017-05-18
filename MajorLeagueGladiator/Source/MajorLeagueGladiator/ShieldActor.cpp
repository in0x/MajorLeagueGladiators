// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "Projectiles/BaseProjectile.h"
#include "MlgPlayerController.h"
#include "MlgGameplayStatics.h"

namespace
{
	const FName REFLECT_SOCKET_NAME("Reflect");
	const FName SHIELD_COLLISION_PRESET_NAME("Shield");
}

AShieldActor::AShieldActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(Super::MESH_COMPONENT_NAME))
{
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(SHIELD_COLLISION_PRESET_NAME);

	UStaticMeshComponent* staticMeshComp = CastChecked<UStaticMeshComponent>(MeshComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/shield09.shield09'"));
	staticMeshComp->SetStaticMesh(staticMesh.Object);

	ConstructorHelpers::FObjectFinder<UMaterialInterface> material(TEXT("Material'/Game/Materials/M_Transparent.M_Transparent'"));
	staticMeshComp->SetMaterial(0, material.Object);

	ConstructorHelpers::FObjectFinder<USoundCue> spawnSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/ShieldSpawnSound_Cue.ShieldSpawnSound_Cue'"));
	spawnSoundCue = spawnSoundCueFinder.Object;
	
	ConstructorHelpers::FObjectFinder<USoundCue> reflefctSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/ShieldReflectSound_Cue.ShieldReflectSound_Cue'"));
	reflectSoundCue = reflefctSoundCueFinder.Object;
}

void AShieldActor::BeginPlay() 
{
	Super::BeginPlay();
	playSpawnSound();
}

FTransform AShieldActor::GetReflectSpawnTransform() const
{
	checkf(MeshComponent->DoesSocketExist(REFLECT_SOCKET_NAME), TEXT("Socket \"Reflect\" is missing in the shield actor mesh"));
	return MeshComponent->GetSocketTransform(REFLECT_SOCKET_NAME);
}

void AShieldActor::PlayReflectSound()
{
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(GetWorld(), FSoundParams(reflectSoundCue, GetActorLocation()));
}

void AShieldActor::playSpawnSound()
{
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(GetWorld(), FSoundParams(spawnSoundCue, GetActorLocation()));
}

void AShieldActor::OnHitInteractable(const ABaseProjectile* projectile)
{
	FTransform socketTransform = GetReflectSpawnTransform();
	projectile->FireProjectile(socketTransform.GetLocation(), socketTransform.Rotator().Vector(), this, Instigator->GetController());
	PlayReflectSound();

	if (AMlgPlayerController* playerController = GetMlgPlayerController())
	{
		playerController->ClientPlayForceFeedback(playerController->GetRumbleShortLeft(), false, FName("rumbleLeft"));
	}
}