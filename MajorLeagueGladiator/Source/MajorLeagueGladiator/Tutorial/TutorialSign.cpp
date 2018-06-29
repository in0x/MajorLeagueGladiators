// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameplayStatics.h"
#include "TutorialSign.h"

namespace
{
	static const FName TextureParamName = FName(TEXT("SignTexture"));
}

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
	
	if (UMlgGameplayStatics::IsUsingDeviceOfType(EHMDDeviceType::DT_OculusRift) && signTextureOculus != nullptr)
	{
		materialInstance->SetTextureParameterValue(::TextureParamName, signTextureOculus);
	}
	else
	{
		materialInstance->SetTextureParameterValue(::TextureParamName, signTexture);
	}
}
