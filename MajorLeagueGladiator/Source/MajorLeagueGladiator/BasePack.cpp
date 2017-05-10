// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BasePack.h"

#include "PackMovementComponent.h"

#include "Characters/MlgPlayerCharacter.h"

namespace
{
	const FName PACK_COLLISION_PROFILE_NAME("Pack");
	const FName PACK_MATERIAL_GLOW_PARAMETER_NAME("Glow");
}

ABasePack::ABasePack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, chargingPlayerClass(nullptr)
	, maxTimeChargedSeconds(10.f)
	, timeChargeLeftSeconds(0.0f)
{
	SetReplicates(true);
	bReplicateMovement = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->bMultiBodyOverlap = true;
	MeshComponent->SetRelativeScale3D({ 0.25,0.25,0.25 });
	MeshComponent->SetCollisionProfileName(PACK_COLLISION_PROFILE_NAME);

	MeshComponent->SetSimulatePhysics(false);
	movementComponent = ObjectInitializer.CreateDefaultSubobject<UPackMovementComponent>(this, TEXT("PackMovementComponent"));
	movementComponent->bShouldBounce = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> packMesh(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (packMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(packMesh.Object);
	}
}

void ABasePack::BeginPlay()
{
	Super::BeginPlay();
	materialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (materialInstance)
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::BeginPlay: Material Missing"));
	}

	setTimeChargeLeftSeonds(0);
}

void ABasePack::setTimeChargeLeftSeonds(float newValue)
{
	timeChargeLeftSeconds = newValue;	
	updateMaterial();	
}

void ABasePack::updateMaterial()
{
	if (materialInstance == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::updateMaterial: Material Instance Missing"));
		return;
	}
	materialInstance->SetScalarParameterValue(PACK_MATERIAL_GLOW_PARAMETER_NAME, timeChargeLeftSeconds);
}

bool ABasePack::canBeChargedBy(const AMlgPlayerCharacter* PlayerCharacter) const
{
	return PlayerCharacter->IsA(chargingPlayerClass);
}



bool ABasePack::IsCharged() const
{
	return timeChargeLeftSeconds != 0.0f;
}

void ABasePack::ChargePackIfPossible(const AMlgPlayerCharacter* PlayerCharacter)
{
	if (canBeChargedBy(PlayerCharacter))
	{
		chargePack();
	}
}

void ABasePack::chargePack()
{
	if (!HasAuthority())
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::chargePack should only be called on Server"));
		return;
	}

	chargePack_NetMulticast();
}


void ABasePack::chargePack_NetMulticast_Implementation()
{
	setTimeChargeLeftSeonds(maxTimeChargedSeconds);
	check(IsCharged());

	OnActorHit.RemoveDynamic(this, &ABasePack::onCollision);
	SetActorTickEnabled(false);
}

void ABasePack::OnLaunch()
{
	if (!HasAuthority())
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::OnThrow should only be called on Server"));
		return;
	}

	if (IsCharged())
	{
		onLaunch_NetMulticast();
	}
}

void ABasePack::onLaunch_NetMulticast_Implementation()
{
	OnActorHit.AddDynamic(this, &ABasePack::onCollision);
}

void ABasePack::onCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	SetActorTickEnabled(true);
	OnActorHit.RemoveDynamic(this, &ABasePack::onCollision);
}

void ABasePack::Tick(float DeltaTime)
{
	const float newTimeChargeLeftSeconds = timeChargeLeftSeconds - DeltaTime;
	if (newTimeChargeLeftSeconds <= 0)
	{
		setTimeChargeLeftSeonds(0);
		SetActorTickEnabled(false);
	}
	else
	{
		setTimeChargeLeftSeonds(newTimeChargeLeftSeconds);
	}
}
