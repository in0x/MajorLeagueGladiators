// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageVisualizerComponent.generated.h"

class UDamageReceiverComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UDamageVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	const char* DAMAGE_VALUE_PARAMETER_NAME = "DamageValue";

	UDamageVisualizerComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	UParticleSystem* particleSystem;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	float materialVisualizationDuration;

	UMaterialInstanceDynamic* matInstance;

	bool isVisualizingMaterial;

	UFUNCTION()
	void onDamageReceived(AActor* DamagedActor);

	UFUNCTION()
	void onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation);
	
	UFUNCTION(NetMulticast, reliable)
	void startMaterialVisualization_NetMulticast();

	UFUNCTION(NetMulticast, reliable)
	void startParticleSystemVisualization_NetMulticast(const FTransform& visualizationTransform);

	void stopMaterialVisualization();
};
