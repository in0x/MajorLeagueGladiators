// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "Projectiles/BaseProjectile.h"
#include "MlgPlayerController.h"
#include "MlgGameplayStatics.h"
#include "Components/PoseableMeshComponent.h"
#include "Animation/BlendSpace1D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSingleNodeInstance.h"

namespace
{
	const FName REFLECT_SOCKET_NAME("Reflect");
	const FName SHIELD_COLLISION_PRESET_NAME("Shield");
}

AShieldActor::AShieldActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponent>(Super::MESH_COMPONENT_NAME))
	, startActiveTime(0)
	, currentActiveTime(0)
	, maxShieldSeconds(3.f) 
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(SHIELD_COLLISION_PRESET_NAME);

	USkeletalMeshComponent* skeletalMeshComp = CastChecked<USkeletalMeshComponent>(MeshComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMesh(TEXT("SkeletalMesh'/Game/MVRCFPS_Assets/Shield/shield.shield'"));
	skeletalMeshComp->SetSkeletalMesh(skeletalMesh.Object);
	ConstructorHelpers::FObjectFinder<UBlendSpace1D> blendAnim(TEXT("BlendSpace1D'/Game/MVRCFPS_Assets/Shield/ShieldCharge.ShieldCharge'"));
	skeletalMeshComp->OverrideAnimationData(blendAnim.Object);

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

	FVector BlendParams(percentLeft, 0.0f, 0.0f);
	
	USkeletalMeshComponent* shieldMesh = CastChecked<USkeletalMeshComponent>(MeshComponent);
	shieldMesh->GetSingleNodeInstance()->SetBlendSpaceInput(BlendParams);
}

void AShieldActor::playSpawnSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), spawnSoundCue, GetActorLocation());
}

void AShieldActor::OnHitInteractable(const ABaseProjectile* projectile)
{
	FTransform socketTransform = GetReflectSpawnTransform();
	projectile->FireProjectile(socketTransform.GetLocation(), socketTransform.GetRotation().GetForwardVector(), this, Instigator->GetController());
	PlayReflectSound();

	if (AMlgPlayerController* playerController = GetMlgPlayerController())
	{
		playerController->ClientPlayForceFeedback(playerController->GetRumbleShortLeft(), false, FName("rumbleLeft"));
	}
}