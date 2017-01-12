// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE_API UHealthComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetCurrentHealth() const;
	float GetMaxHealth() const;

	void IncreaseHealth(float Val);
	void DecreaseHealth(float Val);
	void SetHealthToMax();

private:
	UPROPERTY(EditAnywhere, Category = "Health")
	float maxHealth;
	
	UPROPERTY(EditAnywhere, Replicated, Category = "Health")
	float currentHealth;
};
