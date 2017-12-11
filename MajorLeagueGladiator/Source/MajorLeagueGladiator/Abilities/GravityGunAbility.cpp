#include "MajorLeagueGladiator.h"
#include "GravityGunAbility.h"

#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "GameplayAbilityTargetActor_Cone.h"
#include "AbilityTask_PullTarget.h"
#include "AbilityTask_WaitTargetData.h"
#include "CollisionStatics.h"
#include "BasePack.h"
#include "MlgGameplayStatics.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");

	void SetTether(AMlgPlayerCharacter* mlgCharacter, AActor* actor)
	{
		mlgCharacter->SetTetherTarget_Server_Implementation(actor);
		if (!mlgCharacter->HasAuthority())
		{
			mlgCharacter->SetTetherTarget_Server(actor);
		}
	}

	void SetIsLookingForPullTarget(AMlgPlayerCharacter* mlgCharacter, bool bIsLookingForPullTarget)
	{
		mlgCharacter->SetIsLookingForPullTarget_Server_Implementation(bIsLookingForPullTarget);
		if (!mlgCharacter->HasAuthority())
		{
			mlgCharacter->SetIsLookingForPullTarget_Server(bIsLookingForPullTarget);
		}
	}
}


UGravityGunAbility::UGravityGunAbility(const FObjectInitializer& ObjectInitializer)
	: PullRange(1000)
	, PullSpeed(2000)
	, LaunchVelocity(1700)
	, HalfAngleDegrees(10.f)
	, pullTask(nullptr)
	, searchTask(nullptr)
	, gripController(nullptr)
	, gripControllerMesh(nullptr)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;

	ConstructorHelpers::FObjectFinder<USoundCue> shootSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/GravityGunShootSound_Cue.GravityGunShootSound_Cue'"));
	shootSoundCue = shootSoundCueFinder.Object;
}

void UGravityGunAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (pullTask)
	{
		pullTask->ExternalCancel();
		pullTask = nullptr;
	}
}

void UGravityGunAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (searchTask)
	{
		searchTask->ExternalCancel();
		searchTask = nullptr;
	}
}

void UGravityGunAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	SetGripControllerFromOwner();
	if (!HasGrippedActor())
	{
		SearchAndPull();
	}
	else
	{
		LaunchGrippedActor();
	}
}

void UGravityGunAbility::SearchAndPull()
{
	searchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Search Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Cone::StaticClass());
	searchTask->ValidData.AddDynamic(this, &UGravityGunAbility::OnSearchSuccessful);
	searchTask->Cancelled.AddDynamic(this, &UGravityGunAbility::OnSearchCancelled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!searchTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Cone::StaticClass(), spawnedActor))
	{
		return;
	}

	auto* searchActor = CastChecked<AGameplayAbilityTargetActor_Cone>(spawnedActor);

	searchActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	searchActor->StartLocation.SourceComponent = gripControllerMesh;
	searchActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	searchActor->Range = PullRange;
	searchActor->AutoConfirm = true;
	searchActor->IsVisualizingCone = false;
	searchActor->HalfAngleDegrees = HalfAngleDegrees;

	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());

	searchActor->CollisionChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::GRIPSCAN_TRACE_CHANNEL_NAME);

	searchTask->FinishSpawningActor(this, spawnedActor);

	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	SetIsLookingForPullTarget(owner, true);
}

void UGravityGunAbility::OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	SetIsLookingForPullTarget(owner, false);
	searchTask = nullptr;

	AActor* foundActor = Data.Data[0]->GetActors()[0].Get();

	//This gets spawned on server and client, if he started the ability so that he has a prediction	
	pullTask = UAbilityTask_PullTarget::Create(this, "Pull Actor Task", foundActor, gripController, PullSpeed);
	pullTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorPullFinished);
	pullTask->OnFail.AddUObject(this, &UGravityGunAbility::OnActorPullFailed);
	pullTask->ReadyForActivation();

	auto* mlgCharacter = CastChecked<AMlgPlayerCharacter>(gripController->GetOwner());
	SetTether(mlgCharacter, foundActor);	
}

void UGravityGunAbility::OnSearchCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	SetIsLookingForPullTarget(owner, false);
	searchTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGravityGunAbility::OnActorPullFinished(AActor* pulledActor)
{
	pullTask = nullptr;
	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	if (owner->HasAuthority())
	{
		bool gripSuccess = gripController->TryGrabActor(pulledActor);
		ABasePack* pack = Cast<ABasePack>(pulledActor);
		if (gripSuccess && pack)
		{
			pack->ChargePackIfPossible(owner);
		}
	}
	SetTether(owner, nullptr);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGravityGunAbility::OnActorPullFailed()
{
	pullTask = nullptr;
	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	SetTether(owner, nullptr);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGravityGunAbility::LaunchGrippedActor()
{
	const AMlgPlayerCharacter* player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	FSoundParams soundParams;
	soundParams.Sound = shootSoundCue;

	UMlgGameplayStatics::PlaySoundNetworkedPredicted(player, soundParams);

	player->PlayRumbleLeft();

	if (player->HasAuthority())
	{
		FVector velocity = gripControllerMesh->GetSocketRotation(AIM_SOCKET_NAME).Vector() * LaunchVelocity;
		if (ABasePack* pack = Cast<ABasePack>(gripController->GetGrippedActor()))
		{
			pack->OnLaunch();
		}

		gripController->LaunchActor(velocity, true);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGravityGunAbility::SetGripControllerFromOwner()
{
	AActor* owner = GetOwningActorFromActorInfo();
	AMlgPlayerCharacter* mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);

	gripController = mlgPlayerCharacter->GetMotionController(EControllerHand::Left);
	gripControllerMesh = mlgPlayerCharacter->GetMotionControllerMesh(EControllerHand::Left);
}

bool UGravityGunAbility::HasGrippedActor() const
{
	check(gripController);
	return gripController->GrippedObjects.Num() > 0;
}
