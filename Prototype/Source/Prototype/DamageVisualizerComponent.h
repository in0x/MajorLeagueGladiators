// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "DamageVisualizerComponent.generated.h"

struct FMsgDamageReceived;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UDamageVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UDamageVisualizerComponent();

	virtual void BeginPlay() override;

private:

	FMessageEndpointPtr msgEndpoint;

	void OnDamageReceived(const FMsgDamageReceived& Msg, const IMessageContextRef& Context);
		
	
};
