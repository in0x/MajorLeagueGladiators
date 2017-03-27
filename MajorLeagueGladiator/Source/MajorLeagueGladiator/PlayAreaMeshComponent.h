// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "PlayAreaMeshComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UPlayAreaMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPlayAreaMeshComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void BeginPlay() override;
	void SetIsTargetValid(bool bIsTargetValid);

private:
	UMaterialInterface* material;
};
