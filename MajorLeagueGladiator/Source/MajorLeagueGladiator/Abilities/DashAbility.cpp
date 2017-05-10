#include "MajorLeagueGladiator.h"
#include "DashAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_Raycast.h"
#include "AbilityTask_MoveTo.h"

#include "Weapons/Sword.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
}

UDashAbility::UDashAbility()
	: MaxRange(1000.f)
	, MoveSpeed(1500.f)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UDashAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitForTargetTask && targetingSpawnedActor)
	{
		// Player has released targeting button -> finished picking target
		waitForTargetTask->ExternalConfirm(true);
	}
}

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	cachedPlayer = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UDashAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UDashAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;

	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedActor))
	{
		return;
	}

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedActor);

	auto gripController = cachedPlayer->GetMotionControllerMesh(EControllerHand::Left);
	targetingSpawnedActor->StartLocation.SourceComponent = gripController;
	targetingSpawnedActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingSpawnedActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingSpawnedActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingSpawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingSpawnedActor->MaxRange = MaxRange;

	targetingSpawnedActor->EvalTargetFunc = [](const FHitResult& result)
	{
		auto isNotVertical = FVector::DotProduct(result.ImpactNormal, FVector(0, 0, 1)) > 0.7f;
		auto hitSurface = result.bBlockingHit > 0;
		return isNotVertical && hitSurface;
	};

	waitForTargetTask->FinishSpawningActor(this, spawnedActor);

	cachedPlayer->OnAbilityActivated.Broadcast(StaticClass());
}

void UDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;
	SetSwordAlwaysFastEnough(false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDashAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	SetSwordAlwaysFastEnough(true);

	FVector targetLocation = Data.Data[0]->GetHitResult()->Location;

	moveToTask = UAbilityTask_MoveTo::Create(this, "MoveTo Task", targetLocation, MoveSpeed, cachedPlayer);
	moveToTask->OnLocationReached.AddUObject(this, &UDashAbility::OnLocationReached);
	moveToTask->ReadyForActivation();	

	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	cachedPlayer->OnAbilityUseSuccess.Broadcast(StaticClass(), GetCooldownTimeRemaining());
	cachedPlayer->SetAbilityMoveTargetLocation(targetLocation);
}

void UDashAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	cachedPlayer->OnAbilityUseFail.Broadcast(StaticClass());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDashAbility::OnLocationReached()
{
	cachedPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	cachedPlayer->InvalidateAbilityMoveTargetLocation();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDashAbility::SetSwordAlwaysFastEnough(bool IsAlwaysFastEnough)
{
	if (!cachedPlayer->HasAuthority())
	{
		return;
	}

	if (ASword* sword = Cast<ASword>(cachedPlayer->GetAttachedWeapon()))
	{
		sword->SetIsAlwaysFastEnough(IsAlwaysFastEnough);
	}
}
