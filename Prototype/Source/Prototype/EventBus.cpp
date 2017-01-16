// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "EventBus.h"

#include "Singleton.h"

UEventBus& UEventBus::Get()
{
	return USingleton::Get().GetEventBus();
}

void UEventBus::Fire(PtrToMemberDelegate EventDelegate) const
{
	(this->*EventDelegate).Broadcast();
}
