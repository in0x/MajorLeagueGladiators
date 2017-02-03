#include "MajorLeagueGladiator.h"

#include "GameplayAbilityTargetActor_MotionControl.h"

#include "MlgPlayerCharacter.h"

AGameplayAbilityTargetActor_MotionControl::AGameplayAbilityTargetActor_MotionControl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	PrimaryActorTick.bCanEverTick = true;
}



void AGameplayAbilityTargetActor_MotionControl::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGameplayAbilityTargetActor_MotionControl::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void AGameplayAbilityTargetActor_MotionControl::ConfirmTargetingAndContinue()
{
}

void AGameplayAbilityTargetActor_MotionControl::Tick(float DeltaSeconds)
{
	FVector targetPos = calculateTargetPosition();
	SetActorLocation(targetPos);
}

FVector AGameplayAbilityTargetActor_MotionControl::getPointingDirection() const
{
	//TODO: (FS) Use Interfaced instead of hardcoded casting
	AMlgPlayerCharacter* playerCharacter = Cast<AMlgPlayerCharacter>(SourceActor);
	if (!playerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Is not a Mlg Player Character"));
		return FVector::ZeroVector;
	}
	playerCharacter->LeftMotionController->GetForwardVector();
}

FVector AGameplayAbilityTargetActor_MotionControl::calculateTargetPosition() const
{
	return FVector();
}
