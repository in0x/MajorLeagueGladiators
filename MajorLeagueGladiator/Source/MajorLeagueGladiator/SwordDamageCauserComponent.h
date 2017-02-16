// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageCauserComponent.h"
#include "SwordDamageCauserComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API USwordDamageCauserComponent : public UDamageCauserComponent
{
	GENERATED_BODY()

public:
	USwordDamageCauserComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual bool CanDoDamage() override;
private:
	FVector oldSwingSpeed;

	UPROPERTY(EditAnywhere)
	int threshholdDoDamageSquared;

	//TODO: change sword color changing. THIS IS ONLY TEMPORARY
	UPROPERTY(EditAnywhere)
	UMaterial* materialRedObject;

	UPROPERTY(EditAnywhere)
	UMaterial* materialObject;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* materialRedObject_Dyn;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* materialObject_Dyn;

	// between 0 and 1
	// defines how fast new sword speed influences overall speedvalue
	UPROPERTY(EditAnywhere)
	float slashVelocityLearnRate; 

	void turnDamageOn();
	void turnDamageOff();

	void setMaterialOfOwnerMesh(UMaterialInstanceDynamic* material_Dyn);

	bool canDealDamage;
};
