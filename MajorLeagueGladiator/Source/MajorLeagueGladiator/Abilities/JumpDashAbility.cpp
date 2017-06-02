// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "JumpDashAbility.h"

#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_Raycast.h"
#include "Abilities/GameplayAbilityTargetActor_Cone.h"

#include "Characters/MlgPlayerCharacter.h"

#include "MlgGameplayStatics.h"

#include "AI/MlgAICharacter.h"

#include "Weapons/Sword.h"

#include "Abilities/AbilityTask_MoveTo.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
	const FName MOVE_TO_TASK_NAME("MoveToTask");
}

UJumpDashAbility::UJumpDashAbility()
	: launchVelocity(0,0,1300)
	, minmalJumpHightBeforeDash(500)
	, dashSpeed(1750)
	, maxDashRange(50'000)
	, effectDistance(200)
	, halfEffectAngleDegrees(180)
	, stunRadius(400)
	, stunTimeSeconds(2.0f)
	, waitTargetDataTask(nullptr)
	, spawnedTargetingActor_dash(nullptr)
	, moveToTask(nullptr)
	, cachedCharacter(nullptr)
	, cachedMoveComp(nullptr)
	, landingSoundEffect(nullptr)
	, jumpParticleEffect(nullptr)
	, jumpSoundCue(nullptr)
	, landingParticleEffect(nullptr)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystemRef(TEXT("ParticleSystem'/Game/MobileStarterContent/Particles/P_Explosion.P_Explosion'"));
	jumpParticleEffect = particleSystemRef.Object;
	landingParticleEffect = particleSystemRef.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> soundEffectRef(TEXT("SoundWave'/Game/MobileStarterContent/Audio/Explosion01.Explosion01'"));
	landingSoundEffect = soundEffectRef.Object;

	ConstructorHelpers::FObjectFinder<USoundCue> jumpSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Jump_Dash_Cue.Jump_Dash_Cue'"));
	jumpSoundCue = jumpSoundCueFinder.Object;
}

void UJumpDashAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (spawnedTargetingActor_dash && spawnedTargetingActor_dash->IsConfirmTargetingAllowed() && waitTargetDataTask)
	{	
		waitTargetDataTask->ExternalConfirm(true);
		waitTargetDataTask = nullptr;
		spawnedTargetingActor_dash = nullptr;
	}
}

void UJumpDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Avatar"));
		return;
	}

	cachedCharacter = CastChecked<AMlgPlayerCharacter>(ActorInfo->AvatarActor.Get());
	cachedMoveComp = CastChecked<UCharacterMovementComponent>(cachedCharacter->GetMovementComponent());

	if (cachedMoveComp->MovementMode != MOVE_Walking)
	{
		EndAbility(Handle,ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to activate ability with insufficient resources"));
		return;
	}

	cachedCharacter->SetAbilityMoveTargetLocation(cachedCharacter->GetActorLocation());
	cachedCharacter->LaunchCharacter(launchVelocity, true, true);
	cachedCharacter->MovementModeChangedDelegate.AddDynamic(this, &UJumpDashAbility::OnMovementModeChanged);
	BeginFindingActorsToLaunch();

	const float targetingDelay = minmalJumpHightBeforeDash / launchVelocity.Z;

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.SetTimer(timerHandle, this, &UJumpDashAbility::BeginTargeting,
		targetingDelay, false);

	PlayJumpEffects();
	cachedCharacter->OnAbilityActivated.Broadcast(StaticClass());
	cachedCharacter->OnAbilityUseSuccess.Broadcast(StaticClass(), GetCooldownTimeRemaining());
}

void UJumpDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bWasCancelled)
	{
		return;
	}

	cachedCharacter->MovementModeChangedDelegate.RemoveAll(this);

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.ClearTimer(timerHandle);
	timerHandle.Invalidate();
	
	if (waitTargetDataTask)
	{
		waitTargetDataTask->ValidData.RemoveAll(this);
		waitTargetDataTask->Cancelled.RemoveAll(this);
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}
	if (moveToTask)
	{
		moveToTask->OnLocationReached.RemoveAll(this);
		moveToTask->EndTask();
		moveToTask = nullptr;
	}

	StunEnemiesInArea();
	PlayLandingEffects();
	SetSwordAlwaysFastEnough(false);
}

void UJumpDashAbility::OnLocationReached()
{	
	moveToTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UJumpDashAbility::BeginFindingActorsToLaunch()
{
	UAbilityTask_WaitTargetData* findActorsToLaunchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "findActorsToLaunchTask",
		EGameplayTargetingConfirmation::Instant, AGameplayAbilityTargetActor_Cone::StaticClass());

	findActorsToLaunchTask->ValidData.AddDynamic(this, &UJumpDashAbility::LaunchFoundActorsUpwards);

	AGameplayAbilityTargetActor* spawnedTargetingActor_launch = nullptr;

	if (!findActorsToLaunchTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Cone::StaticClass(), spawnedTargetingActor_launch))
	{
		return;
	}

	AGameplayAbilityTargetActor_Cone* targetingConeActor = CastChecked<AGameplayAbilityTargetActor_Cone>(spawnedTargetingActor_launch);
	targetingConeActor->Range = effectDistance;
	targetingConeActor->HalfAngleDegrees = halfEffectAngleDegrees;
	targetingConeActor->StartLocation = MakeTargetLocationInfoFromOwnerActor();
	targetingConeActor->IsValidTargetFunction = [](AActor* actor) { return Cast<ACharacter>(actor) != nullptr; };
	targetingConeActor->QueryTypes = TArray<TEnumAsByte<EObjectTypeQuery>>{	UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)};

	findActorsToLaunchTask->FinishSpawningActor(this, spawnedTargetingActor_launch);
}

void UJumpDashAbility::LaunchFoundActorsUpwards(const FGameplayAbilityTargetDataHandle& Data)
{
	const FGameplayAbilityTargetData* targetData = Data.Get(0);
	for (const auto& actor : targetData->GetActors())
	{
		if (actor.IsValid())
		{
			ACharacter* characterToLaunch = CastChecked<ACharacter>(actor.Get());
			if (CanLaunchCharacter(characterToLaunch))
			{
				characterToLaunch->LaunchCharacter(launchVelocity, true, true);		
			}		
		}
	}
}

bool UJumpDashAbility::CanLaunchCharacter(ACharacter* Character) const
{
	return UMlgGameplayStatics::CanDealDamageTo(cachedCharacter, Character);
}

void UJumpDashAbility::BeginTargeting()
{
	waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask",
		EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());

	waitTargetDataTask->ValidData.AddDynamic(this, &UJumpDashAbility::OnTargetingSuccess);

	spawnedTargetingActor_dash = nullptr;

	if (!waitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedTargetingActor_dash))
	{
		return;
	}

	AGameplayAbilityTargetActor_Raycast* targetingRayCastActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedTargetingActor_dash);
	
	auto gripController = cachedCharacter->GetMotionControllerMesh(EControllerHand::Left);
	targetingRayCastActor->StartLocation.SourceComponent = gripController;
	targetingRayCastActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingRayCastActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingRayCastActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingRayCastActor->IgnoredActors.Add(cachedCharacter);
	targetingRayCastActor->MaxRange = maxDashRange;
	targetingRayCastActor->CanConfirmInvalidTarget = false;

	targetingRayCastActor->EvalTargetFunc = [](const FHitResult& result)
	{
		auto isNotVertical = FVector::DotProduct(result.ImpactNormal, FVector(0, 0, 1)) > 0.7f;
		auto hitSurface = result.bBlockingHit > 0;
		return isNotVertical && hitSurface;
	};

	waitTargetDataTask->FinishSpawningActor(this, spawnedTargetingActor_dash);
}

void UJumpDashAbility::OnTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data)
{
	waitTargetDataTask = nullptr;
	spawnedTargetingActor_dash = nullptr;

	const FHitResult* hitresult = Data.Data[0]->GetHitResult();
	check(hitresult);

	FVector targetLocation = hitresult->Location;

	const FVector actorLocation = cachedCharacter->GetActorLocation();
	const FVector distance = targetLocation - actorLocation;
	const FVector direction = distance.GetUnsafeNormal();

	float zNormaliseValue = std::abs(1 / direction.Z);

	float realDashSpeed = dashSpeed * zNormaliseValue;
	
	DashTo(targetLocation, realDashSpeed);
}

void UJumpDashAbility::OnMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	auto movementMode = cachedMoveComp->MovementMode;
	if (movementMode == MOVE_Walking)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UJumpDashAbility::DashTo(const FVector& Location, float MoveSpeed)
{
	cachedCharacter->MovementModeChangedDelegate.RemoveAll(this);

	moveToTask = UAbilityTask_MoveTo::Create(this, MOVE_TO_TASK_NAME, Location, MoveSpeed, cachedCharacter);
	moveToTask->OnLocationReached.AddUObject(this, &UJumpDashAbility::OnLocationReached);
	moveToTask->ReadyForActivation();

	SetSwordAlwaysFastEnough(true);
}

void UJumpDashAbility::StunEnemiesInArea()
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	TArray<FOverlapResult> outOverlaps;
	const FTransform ownerTransform = cachedCharacter->GetTransform();
	const FVector ownerLocation = ownerTransform.GetLocation();
	const FQuat ownerRotation = ownerTransform.GetRotation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwningActorFromActorInfo());

	UWorld* world = cachedCharacter->GetWorld();
	world->OverlapMultiByObjectType(
		outOverlaps,
		ownerLocation,
		ownerRotation,
		FCollisionObjectQueryParams(queryTypes),
		FCollisionShape::MakeSphere(stunRadius),
		CollisionParams
		);


	for (const auto& overlap : outOverlaps)
	{
		auto actor = overlap.Actor;
		if (actor.IsValid())
		{
			if (AMlgAICharacter* characterToStun = Cast<AMlgAICharacter>(actor.Get()))
			{
				characterToStun->ApplyStagger(stunTimeSeconds);
			}
		}
	}

}

void UJumpDashAbility::PlayJumpEffects()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (jumpParticleEffect)
	{
		FEmitterSpawnParams emitterParams;
		emitterParams.Transform.SetTranslation(cachedCharacter->CalcFeetPosition());
		emitterParams.Template = jumpParticleEffect;
		emitterParams.bAutoDestroy = true;

		UMlgGameplayStatics::SpawnEmitterNetworkedPredicted(cachedCharacter, emitterParams);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Jump Particle Effect not set"));
	}

	FSoundParams soundParams;
	soundParams.Sound = jumpSoundCue;
	UMlgGameplayStatics::PlaySoundNetworkedPredicted(cachedCharacter, soundParams);		
}

void UJumpDashAbility::PlayLandingEffects()
{
	if (!IsLocallyControlled()) { return; }

	cachedCharacter->PlayRumbleLeft();
	cachedCharacter->PlayRumbleRight();

	if (landingParticleEffect)
	{
		FEmitterSpawnParams emitterParams;
		emitterParams.Transform.SetTranslation(cachedCharacter->CalcFeetPosition());
		emitterParams.Template = landingParticleEffect;
		emitterParams.bAutoDestroy = true;

		UMlgGameplayStatics::SpawnEmitterNetworkedPredicted(cachedCharacter, emitterParams);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Landing Particle Effect not set"));
	}

	if (landingSoundEffect)
	{
		FSoundParams soundParams;
		soundParams.Sound = landingSoundEffect;
		UMlgGameplayStatics::PlaySoundNetworkedPredicted(cachedCharacter, soundParams);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Landing Sound Effect not set"));
	}
}

void UJumpDashAbility::SetSwordAlwaysFastEnough(bool IsAlwaysFastEnough)
{
	if (!cachedCharacter->HasAuthority())
	{
		return;
	}

	if (ASword* sword = Cast<ASword>(cachedCharacter->GetAttachedWeapon()))
	{
		sword->SetIsAlwaysFastEnough(IsAlwaysFastEnough);
	}
}