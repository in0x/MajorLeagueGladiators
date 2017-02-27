// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitscanProjectile.h"
#include "DamageTypes/PlayerDamage.h"
#include "MlgGameplayStatics.h"

#include "ShieldActor.h"

namespace
{
	const char* ABILITY_ACTOR_PROFILE_NAME = "AbilityActor";
}

AHitscanProjectile::AHitscanProjectile()
	: range(1000000.f)
{
 
}

void AHitscanProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire Projectile should not be called on client"));
		return;
	}

	FHitResult hitresult = Trace(ProjectileOwner->GetWorld(), Location, DirectionVector, { ProjectileOwner });

	if (!hitresult.bBlockingHit)
	{
		return;
	}

	AActor* hitActor = hitresult.GetActor();

	if (AShieldActor* interactable = Cast<AShieldActor>(hitActor))
	{
		interactable->OnHitInteractable(this);
	}
	else if (UMlgGameplayStatics::CanDealDamageTo(ProjectileInstigator, hitActor))
	{
		UGameplayStatics::ApplyPointDamage(hitActor, damage, DirectionVector, hitresult, ProjectileInstigator, ProjectileOwner, UPlayerDamage::StaticClass());
	}
}


FHitResult AHitscanProjectile::Trace(UWorld* world, FVector Location, FVector DirectionVector, const TArray<TWeakObjectPtr<AActor>>& IngnoredActors) const
{
	FVector end = Location + DirectionVector * range;

	FCollisionResponseTemplate abilityActorCollisionTemplate;
	bool success = UCollisionProfile::Get()->GetProfileTemplate(FName(ABILITY_ACTOR_PROFILE_NAME), abilityActorCollisionTemplate);
	checkf(success, TEXT("AbilityActor Collision Profile is missing"));	
	
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn),
		UEngineTypes::ConvertToObjectType(abilityActorCollisionTemplate.ObjectType)
	};
	

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IngnoredActors);

	world->LineTraceSingleByObjectType(result, Location, end, queryTypes, CollisionParams);
	DrawDebugDirectionalArrow(world, Location, end, 100.f, FColor::Purple, true, 2.f);
	return result;
}


