// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"
#include "Characters/RangedPlayerCharacter.h"
#include "MlgGameplayStatics.h"

AHealthPack::AHealthPack()
	: amountToRefillUncharged(30.f)
	, amountToRefillCharged(100.f)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> healthPackMat(TEXT("Material'/Game/BluePrints/HealthPackMat.HealthPackMat'"));

	ConstructorHelpers::FObjectFinder<USoundCue> healSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/HealSound_Cue.HealSound_Cue'"));
	healSoundCue = healSoundCueFinder.Object;

	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (staticMeshComp && healthPackMat.Succeeded())
	{
		staticMeshComp->SetMaterial(0, healthPackMat.Object);
	}
	chargingPlayerClass = ARangedPlayerCharacter::StaticClass();
}

void AHealthPack::playHealSound(AActor* user)
{
	const FSoundParams soundParams(healSoundCue, GetActorLocation());
	
	if (user->IsA<APawn>()) // Directly used by enemy.
	{
		UMlgGameplayStatics::PlaySoundNetworked(GetWorld(), soundParams);
	}
	else // Indirectly used by trigger actor of player.
	{
		const APawn* triggerOwner = CastChecked<APawn>(user->GetOwner());
		UMlgGameplayStatics::PlaySoundNetworkedPredicted(triggerOwner, soundParams);
	}
}

void AHealthPack::Use(AActor* User, TriggerType Type)
{
	if (Type != TriggerType::Health)
	{
		return;
	}
	
	UHealthComponent* healthComponent = User->FindComponentByClass<UHealthComponent>();

	if (!healthComponent)
	{
		const AActor* triggerOwner = User->GetOwner();
		healthComponent = triggerOwner->FindComponentByClass<UHealthComponent>();
	}
		
	checkf(healthComponent, TEXT("Owner of health trigger has no healthcomponent, cannot use healthpack."));

	if (healthComponent->GetCurrentHealthPercentage() < 1.f)
	{
		const float amoutToRefill = IsCharged() ? amountToRefillCharged : amountToRefillUncharged;
		healthComponent->IncreaseHealth(amoutToRefill);

		playHealSound(User);
		ReleaseFromGrippedComponent();
		Destroy();
	}
}
