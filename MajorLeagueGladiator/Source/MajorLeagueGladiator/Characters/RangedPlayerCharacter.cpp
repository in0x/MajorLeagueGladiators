// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedPlayerCharacter.h"

ARangedPlayerCharacter::ARangedPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_RangedMultiTool.M_RangedMultiTool'"));
	multiToolMaterial = multiMat.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInterface> bodyMat(TEXT("Material'/Game/Materials/M_DPS.M_DPS'"));
	bodyMaterial = bodyMat.Object;
}

void ARangedPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto instance = UMaterialInstanceDynamic::Create(multiToolMaterial, nullptr);
	auto bodyInstance = UMaterialInstanceDynamic::Create(bodyMaterial, nullptr);
	leftMesh->SetMaterial(0, instance);
	rightMesh->SetMaterial(0, instance);
	headMesh->SetMaterial(0, bodyInstance);
}




