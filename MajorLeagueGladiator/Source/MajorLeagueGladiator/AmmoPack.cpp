// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "AmmoComponent.h"
#include "Characters/MeleePlayerCharacter.h"
#include "MlgGameplayStatics.h"

AAmmoPack::AAmmoPack()
	: amountToRefillUncharged(10)
	, amountToRefillCharged(30)
{
	ConstructorHelpers::FObjectFinder<USoundCue> reloadSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Reload_Cue.Reload_Cue'"));
	reloadSoundCue = reloadSoundCueFinder.Object;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> ammoPackMat(TEXT("Material'/Game/BluePrints/AmmoPackMat.AmmoPackMat'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (staticMeshComp && ammoPackMat.Succeeded())
	{		
		staticMeshComp->SetMaterial(0, ammoPackMat.Object);
	}
	chargingPlayerClass = AMeleePlayerCharacter::StaticClass();
}

void AAmmoPack::Use(AActor* User, TriggerType Type)
{
	if (Type != TriggerType::Ammo)
	{
		return;
	}

	UAmmoComponent* ammoComponent = User->FindComponentByClass<UAmmoComponent>();
	
	APawn* userOwner = CastChecked<APawn>(User->GetOwner());

	if (!ammoComponent)
	{
		ammoComponent = userOwner->FindComponentByClass<UAmmoComponent>();		
	}

	checkf(ammoComponent != nullptr, TEXT("Owner of ammo trigger has no ammocomponent, cannot use ammopack."));

	if (ammoComponent->GetAmmoCount() < ammoComponent->GetMaxAmmoCount())
	{
		const float amoutToRefill = IsCharged() ? amountToRefillCharged : amountToRefillUncharged;			
		ammoComponent->IncreaseAmmo(amoutToRefill);
	
		UMlgGameplayStatics::PlaySoundNetworkedPredicted(userOwner, FSoundParams(reloadSoundCue, GetActorLocation()));
		ReleaseFromGrippedComponent();
		Destroy();
	}
}

