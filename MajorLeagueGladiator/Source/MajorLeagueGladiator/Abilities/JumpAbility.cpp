#include "MajorLeagueGladiator.h"
#include "JumpAbility.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Characters/MlgPlayerCharacter.h"

UJumpAbility::UJumpAbility()
	: PredictProjectileSpeed(1000.f)
	, MaxTimeInFlight(2.f)
	, waitForTargetTask (nullptr)
	, targetingSpawnedActor(nullptr)
	, cachedPlayer(nullptr)
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
	cachedPlayer = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_PredictProjectile::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UJumpAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UJumpAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_PredictProjectile::StaticClass(), spawnedActor))
	{
		return;
	}

	cachedPlayer->OnAbilityActivated.Broadcast(StaticClass());

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
	if (cachedPlayer->HasAuthority())
	{
		cachedPlayer->LaunchCharacter(AGameplayAbilityTargetActor_PredictProjectile::GetVelocityFromTargetDataHandle(Data), true, true);
		cachedPlayer->SetAbilityMoveTargetLocation(Data.Data[0]->GetHitResult()->ImpactPoint);
	}

	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

	cachedPlayer->MovementModeChangedDelegate.AddDynamic(this, &UJumpAbility::OnMovementModeChanged);
	cachedPlayer->OnAbilityUseSuccess.Broadcast(StaticClass(), GetCooldownTimeRemaining());
}

void UJumpAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	cachedPlayer->OnAbilityUseFail.Broadcast(StaticClass());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UJumpAbility::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (Character->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		Character->MovementModeChangedDelegate.RemoveDynamic(this, &UJumpAbility::OnMovementModeChanged);

		cachedPlayer->StopMovementImmediately_NetMulticast();
		cachedPlayer->InvalidateAbilityMoveTargetLocation();

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

