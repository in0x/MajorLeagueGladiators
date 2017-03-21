// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

ASword::ASword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
}
