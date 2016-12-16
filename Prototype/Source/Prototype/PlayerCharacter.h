// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class PROTOTYPE_API APlayerCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;
};
