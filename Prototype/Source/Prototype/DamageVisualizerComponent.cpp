// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageVisualizerComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgDamageReceived.h"


UDamageVisualizerComponent::UDamageVisualizerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageVisualizerComponent::BeginPlay()
{
	Super::BeginPlay();
	msgEndpoint = FMessageEndpoint::Builder("DamageReceiverMessager").Handling<FMsgDamageReceived>(this, &UDamageVisualizerComponent::OnDamageReceived);

	checkf(msgEndpoint.IsValid(), TEXT("Damage Visualizer Component Msg Endpoint invalid"));
	msgEndpoint->Subscribe<FMsgDamageReceived>();
}

void UDamageVisualizerComponent::OnDamageReceived(const FMsgDamageReceived& Msg, const IMessageContextRef& Context)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UDamageVisualizerComponent got FMsgDamageReceived."));
}
