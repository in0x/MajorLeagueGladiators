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
	
	float CurrentHealth() const;
	float MaxHealth() const;

	void DecreaseHealth(float val);
	void RefillHealth();

private:
	UPROPERTY(EditAnywhere, Category = "Health")
	float maxHealth;
	
	UPROPERTY(EditAnywhere, Category = "Health")
	float currentHealth;
};
