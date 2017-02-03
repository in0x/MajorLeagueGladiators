// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageVisualizerComponent.generated.h"

class UDamageReceiverComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UDamageVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UDamageVisualizerComponent();

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	UParticleSystem* particleSystem;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	float materialVisualizationDuration;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
		float particleSystemVisualizationDuration;

	TArray<UParticleSystemComponent*> particleSystemInstances;

	UMaterialInstanceDynamic* matInstance;

	UFUNCTION()
	void onDamageReceived(AActor* DamagedActor);

	UFUNCTION()
	void onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation);
	
	UFUNCTION(NetMulticast, reliable)
	void startMaterialVisualization_NetMulticast();

	UFUNCTION(NetMulticast, reliable)
	void startParticleSystemVisualization_NetMulticast(const FTransform& visualizationTransform);

	void stopMaterialVisualization();

	void stopParticleSystemVisualization();
};
