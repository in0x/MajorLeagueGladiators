// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

#include "MlgGameplayStatics.h"
#include "MlgPlayerController.h"
#include "CollisionStatics.h"

namespace
{
	const char* MELEE_WEAPON_COLLISION_PROFILE_NAME = "MeleeWeapon";
}

//TODO (Flo): FIX multi hit issue

ASword::ASword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, oldSwingSpeed(FVector::ZeroVector)
	, threshholdDoDamageSquared(16)
	, slashVelocityLearnRate(0.1f)
	, isSwordFastEnough(false)
	, damageAppliedOnHit(15.f)
{
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetCollisionProfileName(MELEE_WEAPON_COLLISION_PROFILE_NAME);
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> SwordMaterialRed(TEXT("Material'/Game/MVRCFPS_Assets/Blade_HeroSword22/M_Blade_HeroSword_Red.M_Blade_HeroSword_Red'"));

	if (SwordMaterialRed.Object != NULL)
	{
		materialRedObject = CastChecked<UMaterial>(SwordMaterialRed.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("M_Blade_HeroSword_Red not found."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SwordMaterial(TEXT("Material'/Game/MVRCFPS_Assets/Blade_HeroSword22/M_Blade_HeroSword22.M_Blade_HeroSword22'"));

	if (SwordMaterial.Object != NULL)
	{
		materialObject = CastChecked<UMaterial>(SwordMaterial.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("M_Blade_HeroSword22 not found."));
	}
}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector relativeSwordVelocity = calcRelativeVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashVelocityLearnRate) + (DeltaTime * relativeSwordVelocity) * slashVelocityLearnRate;

	bool newIsSwordFastEnough = oldSwingSpeed.SizeSquared() > threshholdDoDamageSquared;

	if (newIsSwordFastEnough && !isSwordFastEnough)
	{
		onStartSlash();
	}
	else if (!newIsSwordFastEnough && isSwordFastEnough)
	{
		onEndSlash();
	}

	isSwordFastEnough = newIsSwordFastEnough;
}

void ASword::onStartSlash()
{
	damageAllOverlappingActors();

	if (materialRedObject_Dyn == nullptr)
	{
		materialRedObject_Dyn = UMaterialInstanceDynamic::Create(materialRedObject, this);
	}

	setMaterialOfOwnerMesh(materialRedObject_Dyn);
}

void ASword::onEndSlash()
{
	if (!materialObject_Dyn)
	{
		materialObject_Dyn = UMaterialInstanceDynamic::Create(materialObject, this);
	}

	setMaterialOfOwnerMesh(materialObject_Dyn);
}

FHitResult ASword::getOverlappingHit()
{
	FHitResult hitResult;
	FCollisionShape shape;

	const FBoxSphereBounds& Bounds = GetStaticMeshComponent()->Bounds;

	shape.SetBox(Bounds.BoxExtent);
	ECollisionChannel hitScanChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::HITSCAN_TRACE_CHANNEL_NAME);

	GetWorld()->SweepSingleByChannel(hitResult, GetActorLocation(), GetActorLocation(),
		GetActorRotation().Quaternion(), hitScanChannel, shape);

	//FQuat Rotation = GetRootComponent()->GetComponentToWorld().GetRotation();
	//DrawDebugBox(GetWorld(), Bounds.Origin, Bounds.BoxExtent, Rotation, FColor::Red, false, 3, 0);
	//DrawDebugSphere(GetWorld(), hitResult.Location, 5.f, 20, FColor::Cyan, false, 1.5f);

	return hitResult;
}

void ASword::getOverlappingHits(TMap<AActor*, FHitResult>& outActorToHit)
{
	TArray<FHitResult> hitResults;
	FCollisionShape shape;
	const FBoxSphereBounds& Bounds = GetStaticMeshComponent()->Bounds;
	shape.SetBox(Bounds.BoxExtent);
	ECollisionChannel hitScanChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::HITSCAN_TRACE_CHANNEL_NAME);

	GetWorld()->SweepMultiByChannel(hitResults, GetActorLocation(), GetActorLocation(), GetActorRotation().Quaternion(), hitScanChannel, shape);

	AActor* actorToMatch;
	auto pred = [&](FHitResult& hit) 
	{
		return hit.Actor == actorToMatch;
	};

	for (auto& pair : outActorToHit)
	{
		actorToMatch = pair.Key;
		pair.Value = *hitResults.FindByPredicate(pred);
	}
}

void ASword::damageAllOverlappingActors()
{
	TArray <AActor*> overlappingActors;
	AActor* owner = GetOwner();
	owner->GetOverlappingActors(overlappingActors);

	int32 overlaps = overlappingActors.Num();

	TMap<AActor*, FHitResult> actorToHit;

	for (AActor* actor : overlappingActors)
	{
		actorToHit.Add(actor);
	}

	getOverlappingHits(actorToHit);

	bool hasDealtdamage = false;
	for (auto& pair : actorToHit)
	{
		if (UMlgGameplayStatics::CanDealDamageTo(owner, pair.Key))
		{
			dealDamageTo(CastChecked<ACharacter>(pair.Key), pair.Value);
		}
	}
}

void ASword::setMaterialOfOwnerMesh(UMaterialInstanceDynamic* material_Dyn)
{
	auto* mesh = GetStaticMeshComponent();
	mesh->SetMaterial(0, material_Dyn);
}

void ASword::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* otherCharacter = Cast<ACharacter>(OtherActor);
	if (otherCharacter && canDealDamageTo(otherCharacter))
	{
		dealDamageTo(otherCharacter, getOverlappingHit());
	}
}

void ASword::tryLaunchCharacter(ACharacter* character) const
{
	check(character);
	UCharacterMovementComponent* charMoveComp = 
		CastChecked<UCharacterMovementComponent>(character->GetMovementComponent());
	EMovementMode moveMode = charMoveComp->MovementMode;

	if (moveMode == MOVE_Falling)
	{
		charMoveComp->StopMovementImmediately();
		character->LaunchCharacter(GetVelocity(), true, true);
	}
}


void ASword::doRumbleRight()
{	
	AMlgPlayerController* controller = GetMlgPlayerController();
	if (controller != nullptr)
	{		
		controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
	}
	
}

bool ASword::canDealDamageTo(const ACharacter* OtherCharacter) const
{
	check(OtherCharacter)
	return isSwordFastEnough
		&& GetOwner() != OtherCharacter
		&& UMlgGameplayStatics::CanDealDamageTo(this, OtherCharacter);
}

void ASword::dealDamageTo(ACharacter* OtherCharacter, const FHitResult& HitResult)
{
	check(OtherCharacter);

	//UGameplayStatics::ApplyDamage(OtherCharacter, damageAppliedOnHit,
		//GetInstigatorController(), this, damageType);

	UGameplayStatics::ApplyPointDamage(OtherCharacter, damageAppliedOnHit, HitResult.TraceEnd - HitResult.TraceStart, 
		HitResult, GetInstigatorController(), this, damageType);

	doRumbleRight();
	tryLaunchCharacter(OtherCharacter);
}

FVector ASword::calcRelativeVelocity() const
{
	const AActor* owner = GetOwner();
	const FVector ownerVelocity = owner ? owner->GetVelocity() : FVector::ZeroVector;
	const FVector myVelocity = GetVelocity();
	return myVelocity - ownerVelocity;
}
