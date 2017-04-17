#include "MajorLeagueGladiator.h"
#include "JumpAbility.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Characters/MlgPlayerCharacter.h"

UJumpAbility::UJumpAbility()
	: PredictProjectileSpeed(1000.f)
	, MaxTimeInFlight(2.f)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitForTargetTask && targetingSpawnedActor)
	{
		// Player has released targeting button -> finished picking target
		waitForTargetTask->ExternalConfirm(true);
	}
}

void UJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_PredictProjectile::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UJumpAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UJumpAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_PredictProjectile::StaticClass(), spawnedActor))
	{
		return;
	}

	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	player->OnAbilityActivated.Broadcast(StaticClass());

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_PredictProjectile>(spawnedActor);

	targetingSpawnedActor->TargetProjectileSpeed = PredictProjectileSpeed;
	targetingSpawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingSpawnedActor->targetingType = EPickMoveLocationTargeting::FromPlayerCapsule;
	targetingSpawnedActor->TargetProjectileFlightTime = MaxTimeInFlight;

	waitForTargetTask->FinishSpawningActor(this, spawnedActor);
}

void UJumpAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UJumpAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data) 
{
	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	if (player->HasAuthority())
	{
		player->LaunchCharacter(AGameplayAbilityTargetActor_PredictProjectile::GetVelocityFromTargetDataHandle(Data), true, true);
		player->SetAbilityMoveTargetLocation(Data.Data[0]->GetHitResult()->ImpactPoint);
	}

	player->MovementModeChangedDelegate.AddDynamic(this, &UJumpAbility::OnMovementModeChanged);
	player->OnAbilityUseSuccess.Broadcast(StaticClass(), 3.0f);
}

void UJumpAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	player->OnAbilityUseFail.Broadcast(StaticClass());

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UJumpAbility::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (Character->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		Character->MovementModeChangedDelegate.RemoveDynamic(this, &UJumpAbility::OnMovementModeChanged);
	
		AMlgPlayerCharacter* mlgPlayerChar = CastChecked<AMlgPlayerCharacter>(Character);
		mlgPlayerChar->StopMovementImmediately_NetMulticast();
		mlgPlayerChar->InvalidateAbilityMoveTargetLocation();
		
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
