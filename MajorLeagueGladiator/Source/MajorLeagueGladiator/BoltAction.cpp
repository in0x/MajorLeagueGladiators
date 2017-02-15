// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BoltAction.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "VRExpansion/GrippableStaticMeshComponent.h"

ABoltAction::ABoltAction(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
 	PrimaryActorTick.bCanEverTick = true;
	
	VRGripInterfaceSettings.FreeDefaultGripType = EGripCollisionType::InteractiveCollisionWithPhysics;
	VRGripInterfaceSettings.SlotDefaultGripType = EGripCollisionType::InteractiveCollisionWithPhysics;

	auto bolt = CastChecked<UStaticMeshComponent>(GetRootComponent());
	bolt->SetSimulatePhysics(true);
	bolt->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bolt->bGenerateOverlapEvents = true;
	
	baseMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BaseMesh"));
	baseMesh->SetSimulatePhysics(true);
	baseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	baseMesh->bGenerateOverlapEvents = true;
	baseMesh->SetupAttachment(GetRootComponent());

	constraint = ObjectInitializer.CreateDefaultSubobject<UPhysicsConstraintComponent>(this, TEXT("Constraint"));

	FConstraintInstance constraintInst;

	constraintInst.SetLinearXMotion(ELinearConstraintMotion::LCM_Locked);
	constraintInst.SetLinearYMotion(ELinearConstraintMotion::LCM_Limited);
	constraintInst.SetLinearZMotion(ELinearConstraintMotion::LCM_Locked);

	constraintInst.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked);
	constraintInst.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked);
	constraintInst.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Locked);

	constraint->ConstraintInstance = constraintInst;
	constraint->SetConstrainedComponents(baseMesh, NAME_None, CastChecked<UStaticMeshComponent>(GetRootComponent()), NAME_None);

	boltFrontPosition = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BoltFrontPosition"));
	boltFrontPosition->SetupAttachment(baseMesh);
	boltFrontPosition->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boltFrontPosition->bGenerateOverlapEvents = true;
	boltFrontPosition->OnComponentBeginOverlap.AddDynamic(this, &ABoltAction::onFrontBoltPositionOverlap);

	boltBackPosition = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BoltBackPosition"));
	boltBackPosition->SetupAttachment(baseMesh);
	boltBackPosition->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boltBackPosition->bGenerateOverlapEvents = true;
	boltBackPosition->OnComponentBeginOverlap.AddDynamic(this, &ABoltAction::onBackBoltPositionOverlap);
}

void ABoltAction::BeginPlay()
{
	Super::BeginPlay();
	constraint->ConstraintInstance.AngularRotationOffset = FRotator::ZeroRotator;
}

// NOTE(Phil) Lock when not gripped, unlock when gripped (Y axis) -> should fix drifting (this is also why you wanted the actor to inherit from IVRGripInterface)

bool ABoltAction::CompletedPull() const
{
	return bBackPullComplete && bFrontPullComplete;
}

void ABoltAction::ResetPull()
{
	bBackPullComplete = false;
	bFrontPullComplete = false;
}

void ABoltAction::onFrontBoltPositionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Front Bolt cock"));
}

void ABoltAction::onBackBoltPositionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Back Bolt cock"));
}
