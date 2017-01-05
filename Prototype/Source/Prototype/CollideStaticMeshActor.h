// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "CollideStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API ACollideStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ACollideStaticMeshActor();
	UFUNCTION() void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

};
