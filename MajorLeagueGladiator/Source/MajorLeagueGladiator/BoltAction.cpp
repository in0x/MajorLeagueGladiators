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
	
	auto bolt = CastChecked<UGrippableStaticMeshComponent>(GetRootComponent());
	bolt->SetSimulatePhysics(true);
	bolt->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bolt->bGenerateOverlapEvents = true;
	bolt->VRGripInterfaceSettings.FreeDefaultGripType = EGripCollisionType::InteractiveCollisionWithPhysics;
	bolt->VRGripInterfaceSettings.SlotDefaultGripType = EGripCollisionType::InteractiveCollisionWithPhysics;

	baseMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BaseMesh"));
	baseMesh->SetSimulatePhysics(true);
	baseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	baseMesh->bGenerateOverlapEvents = true;
	
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
}

void ABoltAction::BeginPlay()
{
	Super::BeginPlay();	
}

void ABoltAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Lock when not gripped, unlock when gripped (Y axis)

