// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EventData.h"

#include "EventBus.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDelegateWithData, const FEventData&, Data);

UCLASS()
class PROTOTYPE_API UEventBus : public UObject
{
	GENERATED_BODY()
private:
	using PtrToMemberDelegate = FEventDelegate UEventBus::*;
	using PtrToMemberDelegateWithData = FEventDelegateWithData UEventBus::*;

public:
	static UEventBus& Get();

	void Fire(PtrToMemberDelegate EventDelegate) const;

	template<class DATATYPE>
	void Fire(PtrToMemberDelegateWithData EventDelegate, const DATATYPE& Data) const
	{
		(this->*EventDelegate).Broadcast(FEventData::Make(Data));
	}	

	UPROPERTY(BlueprintCallable)
	FEventDelegate CustomEvent;

	UPROPERTY(BlueprintCallable)
	FEventDelegateWithData CustomEventWithData;
};
