// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UHealthComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool IsMainHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;
};
