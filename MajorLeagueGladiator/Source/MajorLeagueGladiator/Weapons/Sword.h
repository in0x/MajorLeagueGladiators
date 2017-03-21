// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/MlgWeapon.h"
#include "Sword.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API ASword : public AMlgWeapon
{
	GENERATED_BODY()	
public:
	ASword(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
