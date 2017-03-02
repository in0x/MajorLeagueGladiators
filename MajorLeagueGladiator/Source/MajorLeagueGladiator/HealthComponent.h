// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "HealthComponent.generated.h"

struct FMsgHealthRefill;
struct FMsgHealthChanged;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UHealthComponent : public USceneComponent
{
	GENERATED_BODY()
		
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FloatDelegate, float, healthPercentage);

public:	
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void BeginPlay();

	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealthPercentage() const;
	float GetMaxHealth() const;

	void IncreaseHealth(float Val);
	void DecreaseHealth(float Val);
	void SetHealthToMax();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FloatDelegate HealthChangedDelegate;

private:
	UPROPERTY(EditAnywhere, Category = "Health")
	float maxHealth;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = onRep_CurrentHealth, Category = "Health")
	float currentHealth;

	FMessageEndpointPtr msgEndpoint;

	void OnHealthRefill(const FMsgHealthRefill& Msg, const IMessageContextRef& Context);

	UFUNCTION()
	void onRep_CurrentHealth();
};
