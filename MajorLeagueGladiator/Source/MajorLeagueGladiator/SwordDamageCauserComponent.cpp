// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SwordDamageCauserComponent.h"
#include "MlgGrippableStaticMeshActor.h"

USwordDamageCauserComponent::USwordDamageCauserComponent()
	: oldSwingSpeed(FVector::ZeroVector)
	, threshholdDoDamageSquared(16)
	, slashVelocityLearnRate(0.1f)
	, canDealDamage(false)
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> SwordMaterialRed(TEXT("Material'/Game/MVRCFPS_Assets/Blade_HeroSword22/M_Blade_HeroSword_Red.M_Blade_HeroSword_Red'"));
	if (SwordMaterialRed.Object != NULL)
	{
		materialRedObject = (UMaterial*)SwordMaterialRed.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> SwordMaterial(TEXT("Material'/Game/MVRCFPS_Assets/Blade_HeroSword22/M_Blade_HeroSword22.M_Blade_HeroSword22'"));
	if (SwordMaterial.Object != NULL)
	{
		materialObject = (UMaterial*)SwordMaterial.Object;
	}
}

void USwordDamageCauserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector newSwordState = GetOwner()->GetVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashVelocityLearnRate) + (DeltaTime * newSwordState) * slashVelocityLearnRate;
	bool newCanDealDamage = oldSwingSpeed.SizeSquared() > threshholdDoDamageSquared;
	if (newCanDealDamage && !canDealDamage)
	{
		turnDamageOn();
	}
	else if (!newCanDealDamage && canDealDamage)
	{
		turnDamageOff();
	}
	canDealDamage = newCanDealDamage;
}

bool USwordDamageCauserComponent::CanDoDamage()
{
	return canDealDamage;
}

void USwordDamageCauserComponent::turnDamageOn() 
{
	if (materialRedObject_Dyn == nullptr)
	{
		materialRedObject_Dyn = UMaterialInstanceDynamic::Create(materialRedObject, this);
	}

	AMlgGrippableStaticMeshActor* owner = Cast<AMlgGrippableStaticMeshActor>(GetOwner());
	if (owner != nullptr)
	{
		auto* mesh = owner->GetStaticMeshComponent();
		mesh->SetMaterial(0, materialRedObject_Dyn);
	}
}

void USwordDamageCauserComponent::turnDamageOff()
{
	if (!materialObject_Dyn)
	{
		materialObject_Dyn = UMaterialInstanceDynamic::Create(materialObject, this);
	}
	AMlgGrippableStaticMeshActor* owner = Cast<AMlgGrippableStaticMeshActor>(GetOwner());
	auto* mesh = owner->GetStaticMeshComponent();
	mesh->SetMaterial(0, materialObject_Dyn);
}
