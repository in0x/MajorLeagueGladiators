// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Singleton.h"


namespace
{
	const char* REFLECT_SOCKET_NAME = "Reflect";
}

USingleton::USingleton(const FObjectInitializer& ObjectInitializer)
{
}

USingleton& USingleton::Get()
{
	USingleton* singleton = CastChecked<USingleton>(GEngine->GameSingleton);
	return *singleton;
}
