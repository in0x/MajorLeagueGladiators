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

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* meshComponent;

	UMaterialInstanceDynamic* matInstance;

	FMessageEndpointPtr msgEndpoint;

	FTimerHandle spawnTimerHandle;

	void onDamageReceived(const FMsgDamageReceived& Msg, const IMessageContextRef& Context);
		
	void startDamageVisualization();

	void stopDamageVisualization();
};
