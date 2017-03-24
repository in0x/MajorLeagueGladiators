// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineMeshComponent.h"
#include "TargetingSplineMeshComponent.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API UTargetingSplineMeshComponent : public USplineMeshComponent
{
	GENERATED_BODY()
	
public:
	UTargetingSplineMeshComponent();
	
	virtual void BeginPlay() override;

	void SetIsTargetValid(bool bIsTargetValid);
	void SetFromProjectilePath(const TArray<FPredictProjectilePathPointData>& Path);

	FVector GetStartPositionWorld() const;
	FVector GetEndPositionWorld() const;
	
private:
	UPROPERTY(EditAnywhere, Category="TargetingSpline")
	UMaterialInterface* material;

	AActor* transformOwner = nullptr;
};
