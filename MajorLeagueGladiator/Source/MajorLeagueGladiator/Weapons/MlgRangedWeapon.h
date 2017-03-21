// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableStaticMeshActor.h"
#include "MlgRangedWeapon.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgRangedWeapon : public AMlgGrippableStaticMeshActor
{
	GENERATED_BODY()
public:
	AMlgRangedWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
