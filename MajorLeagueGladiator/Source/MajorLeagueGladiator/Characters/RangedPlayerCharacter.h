// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgPlayerCharacter.h"
#include "RangedPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API ARangedPlayerCharacter : public AMlgPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ARangedPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
};
