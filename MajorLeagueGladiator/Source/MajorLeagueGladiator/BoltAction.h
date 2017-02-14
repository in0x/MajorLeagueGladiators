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
	
	bool CompletedPull() const;
	void ResetPull();

private:
	// Mesh the bolt is attached to.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;
	
	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* constraint;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* boltFrontPosition;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* boltBackPosition;

	UFUNCTION()
	void onFrontBoltPositionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void onBackBoltPositionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bBackPullComplete;
	bool bFrontPullComplete;
};
