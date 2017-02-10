// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitScanGunActor.h"
#include "DamageTypes/PlayerDamage.h"

AHitScanGunActor::AHitScanGunActor()
{
}

void AHitScanGunActor::BeginPlay()
{
	auto mesh = GetStaticMeshComponent()->GetStaticMesh();
	if (mesh)
	{
		shotOriginSocket = mesh->FindSocket(FName("ProjectileSpawn"));

		if (!shotOriginSocket)
		{
			UE_LOG(DebugLog, Warning, TEXT("Gun Mesh does not have a 'ProjectileSpawn' socket, shots will originate from incorrect position"));
		}
	}
}

void AHitScanGunActor::OnUsed()
{
	shoot();
}

void AHitScanGunActor::shoot() 
{
	FTransform trafo;
	shotOriginSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

	auto forward = trafo.GetRotation().GetForwardVector();
	forward.Normalize();
	auto start = trafo.GetLocation();
	auto end = start + forward * shotRange; 
	
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	auto* world = GetWorld();
	
	world->LineTraceSingleByObjectType(result, start, end, queryTypes, CollisionParams);
	DrawDebugLine(world, start, end, FColor::Red);

	if (result.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(result.GetActor(), damage, nullptr, this, UPlayerDamage::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("shot"));
	}
}





