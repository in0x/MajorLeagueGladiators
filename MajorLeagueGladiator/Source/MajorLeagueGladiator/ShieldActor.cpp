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
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponent>(Super::MESH_COMPONENT_NAME))
	, maxCurrentActiveTime(0)
	, currentActiveTime(0)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(SHIELD_COLLISION_PRESET_NAME);

	USkeletalMeshComponent* skeletalMeshComp = CastChecked<USkeletalMeshComponent>(MeshComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMesh(TEXT("SkeletalMesh'/Game/MVRCFPS_Assets/Shield/shield.shield'"));
	skeletalMeshComp->SetSkeletalMesh(skeletalMesh.Object);

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

void AShieldActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	currentActiveTime += DeltaSeconds;
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

void AShieldActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShieldActor, maxCurrentActiveTime)
}

void AShieldActor::playSpawnSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), spawnSoundCue, GetActorLocation());
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