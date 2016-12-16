// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PrototypeGameModeBase.h"
#include "PlayerCharacter.h"

APrototypeGameModeBase::APrototypeGameModeBase(const FObjectInitializer& ObjectInitializer)
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
}
