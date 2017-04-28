// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "JumpDashAbility.h"

#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_Raycast.h"
#include "Abilities/GameplayAbilityTargetActor_Cone.h"

#include "Characters/MlgPlayerCharacter.h"

#include "MlgGameplayStatics.h"

#include "AI/MlgAICharacter.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
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
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystemRef(TEXT("ParticleSystem'/Game/MobileStarterContent/Particles/P_Explosion.P_Explosion'"));
	jumpParticleEffect = particleSystemRef.Object;
	landingParticleEffect = particleSystemRef.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> soundEffectRef(TEXT("SoundWave'/Game/MobileStarterContent/Audio/Explosion01.Explosion01'"));
	jumpSoundEffect = soundEffectRef.Object;
	landingSoundEffect = soundEffectRef.Object;
}

void UJumpDashAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitTargetDataTask)
	{
		waitTargetDataTask->ExternalConfirm(true);
		waitTargetDataTask = nullptr;
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
	cachedCharacter->OnActorHit.AddDynamic(this, &UJumpDashAbility::OnCollidedWithWorld);
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

	cachedMoveComp->SetMovementMode(MOVE_Falling);
	cachedMoveComp->StopMovementImmediately();

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.ClearTimer(timerHandle);
	timerHandle.Invalidate();

	cachedCharacter->OnActorHit.RemoveDynamic(this, &UJumpDashAbility::OnCollidedWithWorld);
	
	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}
	PlayLandingEffects();
	cachedCharacter->InvalidateAbilityMoveTargetLocation();
}

void UJumpDashAbility::OnCollidedWithWorld(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{	
	cachedCharacter->OnActorHit.RemoveDynamic(this, &UJumpDashAbility::OnCollidedWithWorld);

	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}

	StunEnemiesInArea();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UJumpDashAbility::BeginFindingActorsToLaunch()
{
	UAbilityTask_WaitTargetData* findActorsToLaunchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "findActorsToLaunchTask",
		EGameplayTargetingConfirmation::Instant, AGameplayAbilityTargetActor_Cone::StaticClass());

	findActorsToLaunchTask->ValidData.AddDynamic(this, &UJumpDashAbility::LaunchFoundActorsUpwards);

	AGameplayAbilityTargetActor* spawnedTargetingActor = nullptr;

	if (!findActorsToLaunchTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Cone::StaticClass(), spawnedTargetingActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_Cone* targetingConeActor = CastChecked<AGameplayAbilityTargetActor_Cone>(spawnedTargetingActor);
	targetingConeActor->Range = effectDistance;
	targetingConeActor->HalfAngleDegrees = halfEffectAngleDegrees;
	targetingConeActor->StartLocation = MakeTargetLocationInfoFromOwnerActor();
	targetingConeActor->IsValidTargetFunction = [](AActor* actor) { return Cast<ACharacter>(actor) != nullptr; };
	targetingConeActor->QueryTypes = TArray<TEnumAsByte<EObjectTypeQuery>>{	UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)};

	findActorsToLaunchTask->FinishSpawningActor(this, spawnedTargetingActor);
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
	waitTargetDataTask->Cancelled.AddDynamic(this, &UJumpDashAbility::OnTargetingFailed);

	AGameplayAbilityTargetActor* spawnedTargetingActor = nullptr;

	if (!waitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedTargetingActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_Raycast* targetingRayCastActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedTargetingActor);
	
	auto gripController = cachedCharacter->GetMotionControllerMesh(EControllerHand::Left);
	targetingRayCastActor->StartLocation.SourceComponent = gripController;
	targetingRayCastActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingRayCastActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingRayCastActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingRayCastActor->IgnoredActors.Add(cachedCharacter);
	targetingRayCastActor->MaxRange = maxDashRange;

	targetingRayCastActor->EvalTargetFunc = [](const FHitResult& result)
	{
		auto isNotVertical = FVector::DotProduct(result.ImpactNormal, FVector(0, 0, 1)) > 0.7f;
		auto hitSurface = result.bBlockingHit > 0;
		return isNotVertical && hitSurface;
	};

	waitTargetDataTask->FinishSpawningActor(this, spawnedTargetingActor);
}

void UJumpDashAbility::OnTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data)
{
	const FHitResult* hitresult = Data.Data[0]->GetHitResult();
	check(hitresult);

	const FVector targetLocation = hitresult->Location;
	const FVector actorFeetLocation = cachedCharacter->CalcFeetPosition();
	const FVector feetToTargetVector = targetLocation - actorFeetLocation;
	const FVector direction = feetToTargetVector.GetUnsafeNormal();
	const FVector zNormalizedDirection = direction / std::abs(direction.Z);
	const FVector velocity = zNormalizedDirection * dashSpeed;
	
	cachedCharacter->SetAbilityMoveTargetLocation(targetLocation);
	BeginDashing(velocity);
}

void UJumpDashAbility::OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
	cachedCharacter->OnAbilityUseFail.Broadcast(StaticClass());
	BeginDashing(FVector(0, 0, -dashSpeed));
}

void UJumpDashAbility::BeginDashing(const FVector& Velocity)
{
	cachedMoveComp->StopMovementImmediately();
	cachedMoveComp->SetMovementMode(MOVE_Flying);
	cachedMoveComp->AddImpulse(Velocity, true);
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

	if (jumpSoundEffect)
	{
		FSoundParams soundParams;
		soundParams.Sound = jumpSoundEffect;
		UMlgGameplayStatics::PlaySoundAtLocationNetworkedPredicted(cachedCharacter, soundParams);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Jump Sound Effect not set"));
	}
		
}

void UJumpDashAbility::PlayLandingEffects()
{
	if (!IsLocallyControlled()) { return; }

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

	if (jumpSoundEffect)
	{
		FSoundParams soundParams;
		soundParams.Sound = landingSoundEffect;
		UMlgGameplayStatics::PlaySoundAtLocationNetworkedPredicted(cachedCharacter, soundParams);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Landing Sound Effect not set"));
	}
}
