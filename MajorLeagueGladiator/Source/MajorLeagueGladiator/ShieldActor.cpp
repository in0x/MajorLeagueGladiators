// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "Projectiles/BaseProjectile.h"
#include "MlgPlayerController.h"
#include "MlgGameplayStatics.h"
#include "Components/PoseableMeshComponent.h"

namespace
{
	const FName REFLECT_SOCKET_NAME("Reflect");
	const FName SHIELD_COLLISION_PRESET_NAME("Shield");
	const FName CENTER_BONE_NAME("middle_JT");
}

AShieldActor::AShieldActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPoseableMeshComponent>(Super::MESH_COMPONENT_NAME))
	, startActiveTime(0)
	, currentActiveTime(0)
	, maxShieldSeconds(3.f)
	, centerMaxScale(5.f)
	, centerMinScale(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(SHIELD_COLLISION_PRESET_NAME);

	UPoseableMeshComponent* skeletalMeshComp = CastChecked<UPoseableMeshComponent>(MeshComponent);

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
	currentActiveTime = startActiveTime;
}

void AShieldActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	currentActiveTime += DeltaSeconds;
	if (currentActiveTime >= maxShieldSeconds)
	{
		OnTimeRunOut.ExecuteIfBound();
	}
	else
	{
		updateAnimation(CalcTimeLeft());
	}
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

float AShieldActor::CalcTimeLeft() const
{
	const float timeLeft = maxShieldSeconds - currentActiveTime;
	return FMath::Max(timeLeft, 0.f);
}

float AShieldActor::CalcSecondsUntilRecharged() const
{
	// this means it reacharges as fast as it depletes
	return currentActiveTime;
}

void AShieldActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShieldActor, startActiveTime)
}

void AShieldActor::updateAnimation(float timeRemaining)
{
	const float percentLeft = timeRemaining / maxShieldSeconds;
	const float scale = (centerMaxScale - centerMinScale) * percentLeft + centerMinScale;

	const FVector scaleVector(1, scale, scale);
	UPoseableMeshComponent* shieldMesh = CastChecked<UPoseableMeshComponent>(MeshComponent);
	shieldMesh->SetBoneScaleByName(CENTER_BONE_NAME, scaleVector, EBoneSpaces::ComponentSpace);
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