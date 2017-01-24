// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "Singleton.h"


USingleton::USingleton(const FObjectInitializer& ObjectInitializer)
{
}

USingleton& USingleton::Get()
{
	USingleton* singleton = CastChecked<USingleton>(GEngine->GameSingleton);
	return *singleton;
}
