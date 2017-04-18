#include "MajorLeagueGladiator.h"
#include "DashAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_Raycast.h"
#include "AbilityTask_MoveTo.h"

namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
	const float TEMPORARY_COOLDOWN = 3.0f;
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
	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UDashAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UDashAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedActor))
	{
		return;
	}

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedActor);

	cachedPlayer = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
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

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDashAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	if (cachedPlayer->HasAuthority())
	{
		auto capsule = cachedPlayer->GetCapsuleComponent();

		// Add capsule halfheight to Z since our "location" is the capsule 
		// center, otherwise we would move our body into the ground.
		auto location = Data.Data[0]->GetHitResult()->Location;
		location += capsule->GetUpVector() * capsule->GetScaledCapsuleHalfHeight();

		moveToTask = UAbilityTask_MoveTo::Create(this, "MoveTo Task", location, MoveSpeed, cachedPlayer);
		moveToTask->Activate();
		moveToTask->OnLocationReached.AddUObject(this, &UDashAbility::OnLocationReached);

		cachedPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		cachedPlayer->EnableActorCollison_NetMulticast(false);
		cachedPlayer->SetAbilityMoveTargetLocation(location);
	}

	cachedPlayer->OnAbilityUseSuccess.Broadcast(StaticClass(), TEMPORARY_COOLDOWN);
}

void UDashAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	cachedPlayer->OnAbilityUseFail.Broadcast(StaticClass());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDashAbility::OnLocationReached()
{
	cachedPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	cachedPlayer->EnableActorCollison_NetMulticast(true);
	cachedPlayer->InvalidateAbilityMoveTargetLocation();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

