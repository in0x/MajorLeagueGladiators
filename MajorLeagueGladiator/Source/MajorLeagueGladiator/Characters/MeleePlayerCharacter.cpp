// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleePlayerCharacter.h"

AMeleePlayerCharacter::AMeleePlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_MeleeMultiTool.M_MeleeMultiTool'"));
	multiToolMaterial = multiMat.Object;
}

void AMeleePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto instance = UMaterialInstanceDynamic::Create(multiToolMaterial, nullptr);
	leftMesh->SetMaterial(0, instance);
	rightMesh->SetMaterial(0, instance);
}

