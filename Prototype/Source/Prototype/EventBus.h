// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EventData.h"
#include "EventBus.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEventDelegate, const FEventData&);

UCLASS()
class PROTOTYPE_API UEventBus : public UObject
{
	GENERATED_BODY()
private:
	using PtrToMemberDelegate = FEventDelegate UEventBus::*;

public:
	static UEventBus& Get();

	void Fire(PtrToMemberDelegate EventDelegate) const;

	template<class DATATYPE>
	void Fire(PtrToMemberDelegate EventDelegate, const DATATYPE& Data) const
	{
		(this->*EventDelegate).Broadcast(FEventData::Make(Data));
	}	

	FEventDelegate CustomEvent;

	FEventDelegate AmmoRefillEvent; // Sent with std::tuple<Actor*, uint32> -> TriggerActor and ammo amount
};
