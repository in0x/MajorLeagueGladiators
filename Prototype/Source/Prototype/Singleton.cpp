// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "Singleton.h"

#include "EventBus.h"


USingleton::USingleton(const FObjectInitializer& ObjectInitializer)
	: eventBus(ObjectInitializer.CreateDefaultSubobject<UEventBus>(this, TEXT("Eventbus")))
{}

USingleton& USingleton::Get()
{
	USingleton* singleton = CastChecked<USingleton>(GEngine->GameSingleton);
	return *singleton;
}
