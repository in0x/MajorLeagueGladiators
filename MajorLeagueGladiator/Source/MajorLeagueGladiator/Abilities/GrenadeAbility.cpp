// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GrenadeAbility.h"

#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_PredictProjectile.h"
#include "TargetingSplineMeshComponent.h"

#include "Projectiles/GrenadeProjectile.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgGrippableMeshActor.h"

namespace
{
	const char* SHOT_SOCKET_NAME = "ProjectileSpawn";
}

UGrenadeAbility::UGrenadeAbility()
	: waitTargetDataTask(nullptr)
	, grenadeDefaultObject(nullptr)
	, cachedPlayer(nullptr)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UGrenadeAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitTargetDataTask)
	{
		// Player has released targeting button -> finished picking target
		waitTargetDataTask->ExternalConfirm(true);
		waitTargetDataTask = nullptr;
	}
}

void UGrenadeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Avatar"));
		return;
	}

	grenadeDefaultObject = CastChecked<AGrenadeProjectile>(AGrenadeProjectile::StaticClass()->GetDefaultObject());
	cachedPlayer = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	beginTargeting();
	cachedPlayer->OnAbilityActivated.Broadcast(StaticClass());
}

void UGrenadeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bWasCancelled)
	{
		return;
	}

	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}
}

void UGrenadeAbility::beginTargeting()
{
	waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask",
		EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_PredictProjectile::StaticClass());

	waitTargetDataTask->ValidData.AddDynamic(this, &UGrenadeAbility::onTargetingSuccess);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGrenadeAbility::onTargetingFailed);

	AGameplayAbilityTargetActor* spawnedTargetingActor = nullptr;

	if (!waitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_PredictProjectile::StaticClass(), spawnedTargetingActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_PredictProjectile* targetingActor = CastChecked<AGameplayAbilityTargetActor_PredictProjectile>(spawnedTargetingActor);

	auto weapon = cachedPlayer->GetAttachedWeapon();
	targetingActor->targetingType = EPickMoveLocationTargeting::FromSourceComponent;
	targetingActor->StartLocation.SourceComponent = weapon->MeshComponent;
	targetingActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingActor->StartLocation.SourceSocketName = SHOT_SOCKET_NAME;
	targetingActor->TargetProjectileSpeed = grenadeDefaultObject->InitialSpeed;
	targetingActor->TargetProjectileFlightTime = 0.05f;

	targetingActor->bShowPlayArea = false;
	targetingActor->SetTargetHitColor(FLinearColor::Blue);
	targetingActor->SetTargetMissColor(FLinearColor::Blue);
	
	waitTargetDataTask->FinishSpawningActor(this, spawnedTargetingActor);

	targetingActor->TargetingSplineMesh->SetStartScale(FVector2D(10.f, 10.f));
	targetingActor->TargetingSplineMesh->SetEndScale(FVector2D(10.f, 10.f));
	targetingActor->OverrideGravityZ = GetWorld()->GetDefaultGravityZ() * grenadeDefaultObject->FindComponentByClass<UProjectileMovementComponent>()->ProjectileGravityScale;
}

void UGrenadeAbility::onTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data)
{
	cachedPlayer->PlayRumbleRight();
	fireGrenade();
}

void UGrenadeAbility::onTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
	fireGrenade();
}

// We only use the targeting as a preview here, so we call this both on 
// success and fail. Fail is the more likely case since the grenade will fly 
// much farther than the short targeting draw.
void UGrenadeAbility::fireGrenade()
{
	if (cachedPlayer->HasAuthority())
	{
		auto* weapon = cachedPlayer->GetAttachedWeapon();
		const FTransform targetingTransform = weapon->MeshComponent->GetSocketTransform(SHOT_SOCKET_NAME);
		
		grenadeDefaultObject->FireProjectile(targetingTransform.GetLocation(), targetingTransform.GetRotation().GetForwardVector(), weapon, weapon->Instigator->GetController());
	}

	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	cachedPlayer->OnAbilityUseSuccess.Broadcast(StaticClass(), GetCooldownTimeRemaining());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}



