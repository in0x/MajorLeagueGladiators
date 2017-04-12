// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableMeshActor.h"

const FName AMlgGrippableMeshActor::MESH_COMPONENT_NAME(TEXT("Mesh Component"));

AMlgGrippableMeshActor::AMlgGrippableMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UMeshComponent, UStaticMeshComponent>(this, MESH_COMPONENT_NAME);
	RootComponent = MeshComponent;
}
