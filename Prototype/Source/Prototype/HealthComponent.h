// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "Core.h"
#include "HealthComponent.generated.h"

struct FMsgHealthChanged;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE_API UHealthComponent : public USceneComponent
{
	GENERATED_BODY()
		
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FloatDelegate, float, healthPercentage);

public:	
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetCurrentHealth() const;
	float GetMaxHealth() const;

	void IncreaseHealth(float Val);
	void DecreaseHealth(float Val);
	void SetHealthToMax();

	FloatDelegate HealthChangedDelegate;

private:
	UPROPERTY(EditAnywhere, Category = "Health")
	float maxHealth;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CurrentHealth, Category = "Health")
	float currentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();
};
