// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleePlayerCharacter.h"

AMeleePlayerCharacter::AMeleePlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_MeleeMultiTool.M_MeleeMultiTool'"));
	multiToolMaterial = multiMat.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInterface> swordMat(TEXT("Material'/Game/Materials/M_Sword.M_Sword'"));
	swordMaterial = swordMat.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInterface> bodyMat(TEXT("Material'/Game/Materials/M_Tank.M_Tank'"));
	bodyMaterial = bodyMat.Object;
}

void AMeleePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto instance = UMaterialInstanceDynamic::Create(multiToolMaterial, nullptr);
	auto swordInstance = UMaterialInstanceDynamic::Create(swordMaterial, nullptr);
	auto bodyInstance = UMaterialInstanceDynamic::Create(bodyMaterial, nullptr);
	leftMesh->SetMaterial(0, instance);
	rightMesh->SetMaterial(0, swordInstance);
	headMesh->SetMaterial(0, bodyInstance);
}

