// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "BoltAction.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API ABoltAction : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	ABoltAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// Mesh the bolt is attached to.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;
	
	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* constraint;
};
