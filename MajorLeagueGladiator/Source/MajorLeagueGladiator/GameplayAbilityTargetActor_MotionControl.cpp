#include "MajorLeagueGladiator.h"

#include "GameplayAbilityTargetActor_MotionControl.h"

#include "MlgPlayerCharacter.h"

AGameplayAbilityTargetActor_MotionControl::AGameplayAbilityTargetActor_MotionControl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
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
	check(ShouldProduceTargetData());

	if (SourceActor)
	{
		bDebug = false;		
		FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, hitresult);

		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

void AGameplayAbilityTargetActor_MotionControl::Tick(float DeltaSeconds)
{
	hitresult = calculateHitResult();
	FVector EndPoint = hitresult.Component.IsValid() ? hitresult.ImpactPoint : hitresult.TraceEnd;

	SetActorLocationAndRotation(EndPoint,SourceActor->GetActorRotation());
}

//TODO: Use Strategy Pattern for this
FHitResult AGameplayAbilityTargetActor_MotionControl::calculateHitResult() const
{
	//TODO: (FS) Use Interfaced instead of hardcoded casting
	AMlgPlayerCharacter* playerCharacter = Cast<AMlgPlayerCharacter>(SourceActor);
	if (!playerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Is not a Mlg Player Character"));
		return FHitResult();
	}

	const USceneComponent* projectileThrowingComponent = playerCharacter->LeftMotionController;

	const FVector direction = projectileThrowingComponent->GetForwardVector();
	const FVector location = projectileThrowingComponent->GetComponentLocation();
	const float velocity = 750.f;

	static const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic) };

	FHitResult result;
	FVector lastTraceDest;
	FHitResult tpHitResult;
	TArray<FVector> positions;

	UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		result,
		positions,
		lastTraceDest,
		location,
		direction * velocity,
		true,
		0,
		queryTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		1.f
		);

	return result;
}
