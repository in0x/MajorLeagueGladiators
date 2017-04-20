// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SwordDamageCauserComponent.h"
#include "MlgGrippableMeshActor.h"
#include "MlgPlayerController.h"
#include "MlgGameplayStatics.h"

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
		materialRedObject = CastChecked<UMaterial>(SwordMaterialRed.Object);
	}
	else 
	{
		UE_LOG(DebugLog, Warning, TEXT("M_Blade_HeroSword_Red not found."));
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> SwordMaterial(TEXT("Material'/Game/MVRCFPS_Assets/Blade_HeroSword22/M_Blade_HeroSword22.M_Blade_HeroSword22'"));

	if (SwordMaterial.Object != NULL)
	{
		materialObject = CastChecked<UMaterial>(SwordMaterial.Object);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("M_Blade_HeroSword22 not found."));
	}
}

void USwordDamageCauserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector newSwordState = GetOwner()->GetVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashVelocityLearnRate) + (DeltaTime * newSwordState) * slashVelocityLearnRate;

	bool newCanDealDamage = oldSwingSpeed.SizeSquared() > threshholdDoDamageSquared;

	if (newCanDealDamage && !canDealDamage)
	{
		onStartSlash();
	}
	else if (!newCanDealDamage && canDealDamage)
	{
		onEndSlash();
	}

	canDealDamage = newCanDealDamage;
}

void USwordDamageCauserComponent::onStartSlash()
{
	damageAllOverlappingActors();

	if (materialRedObject_Dyn == nullptr)
	{
		materialRedObject_Dyn = UMaterialInstanceDynamic::Create(materialRedObject, this);
	}

	setMaterialOfOwnerMesh(materialRedObject_Dyn);
}

void USwordDamageCauserComponent::onEndSlash()
{
	if (!materialObject_Dyn)
	{
		materialObject_Dyn = UMaterialInstanceDynamic::Create(materialObject, this);
	}

	setMaterialOfOwnerMesh(materialObject_Dyn);
}

void USwordDamageCauserComponent::damageAllOverlappingActors()
{
	TArray <AActor*> overlappingActors;
	AActor* owner = GetOwner();
	owner->GetOverlappingActors(overlappingActors);

	int32 overlaps = overlappingActors.Num();

	bool hasDealtdamage = false;
	for (AActor* actor : overlappingActors)
	{
		if (UMlgGameplayStatics::CanDealDamageTo(owner, actor))
		{
			UGameplayStatics::ApplyDamage(actor, damageAppliedOnHit, owner->GetInstigatorController(), owner, DamageType);
			hasDealtdamage = true;
		}
	}

	if (hasDealtdamage)
	{
		doRumbleRight(GetOwner());
	}
}

void USwordDamageCauserComponent::setMaterialOfOwnerMesh(UMaterialInstanceDynamic* material_Dyn) 
{
	UMeshComponent* ownerMeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();

	if (ownerMeshComponent)
	{
		ownerMeshComponent->SetMaterial(0, material_Dyn);
	}
}

void USwordDamageCauserComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (canDealDamage && UMlgGameplayStatics::CanDealDamageTo(OverlappedActor, OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, OverlappedActor->GetInstigatorController(), OverlappedActor, DamageType);
		doRumbleRight(OtherActor);
	}
}

void USwordDamageCauserComponent::doRumbleRight(AActor* OtherActor)
{
	AMlgGrippableMeshActor* owner = Cast<AMlgGrippableMeshActor>(GetOwner());
	if (owner != nullptr)
	{
		AMlgPlayerController* controller = owner->GetMlgPlayerController();
		if (controller != nullptr)
		{
			//check if hit object is part of myself, then we do not rumble
			APawn* otherPlayer = Cast<APawn>(OtherActor);
			if (otherPlayer != nullptr)
			{
				if (otherPlayer->Controller == controller)
				{
					return;
				}
			}
			controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
		}
	}
}