// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MlgGrippableActor.h"

#include "MlgGrippableMeshActor.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGrippableMeshActor : public AMlgGrippableActor
{
	GENERATED_BODY()
	
public:	
	static const FName MESH_COMPONENT_NAME;

	AMlgGrippableMeshActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	UMeshComponent* MeshComponent;
};
