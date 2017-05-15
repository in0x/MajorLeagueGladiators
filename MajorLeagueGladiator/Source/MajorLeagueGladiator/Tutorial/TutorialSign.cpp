// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TutorialSign.h"

ATutorialSign::ATutorialSign()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> planeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	GetStaticMeshComponent()->SetStaticMesh(planeMesh.Object);
	
	ConstructorHelpers::FObjectFinder<UMaterialInterface> material(TEXT("Material'/Game/Materials/SignMaterial.SignMaterial'"));
	materialInterface = material.Object;
}

void ATutorialSign::BeginPlay()
{
	Super::BeginPlay();
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	
	mesh->SetMaterial(0, materialInterface);
	UMaterialInstanceDynamic* materialInstance = mesh->CreateAndSetMaterialInstanceDynamic(0);
	materialInstance->SetTextureParameterValue(FName(TEXT("SignTexture")), signTexture);
}
