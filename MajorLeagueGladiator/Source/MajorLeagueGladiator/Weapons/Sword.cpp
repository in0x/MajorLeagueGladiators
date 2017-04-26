// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

#include "MlgGameplayStatics.h"
#include "MlgPlayerController.h"
#include "CollisionStatics.h"

//TODO (Flo): FIX multi hit issue

namespace
{
	const char* MELEE_WEAPON_COLLISION_PROFILE_NAME = "MeleeWeapon";
}

ASword::ASword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, oldSwingSpeed(FVector::ZeroVector)
	, threshholdDoDamageSquared(2)
	, slashVelocityLearnRate(0.1f)
	, bIsSwordFastEnough(false)
	, damageAppliedOnHit(40.f)
{
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(MELEE_WEAPON_COLLISION_PROFILE_NAME);
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SwordStaticMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/MultiTool/sword/sword_blade_3.sword_blade_3'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (SwordStaticMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(SwordStaticMesh.Object);
	}

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

	if (newIsSwordFastEnough && !bIsSwordFastEnough)
	{
		onStartSlash();
	}
	else if (!newIsSwordFastEnough && bIsSwordFastEnough)
	{
		onEndSlash();
	}

	bIsSwordFastEnough = newIsSwordFastEnough;
}

void ASword::onStartSlash()
{
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

void ASword::getOverlappingHits(TArray<TPair<AActor*, FHitResult>>& outActorToHit) const
{
	TArray<FHitResult> hitResults;

	FVector meshLoc = MeshComponent->GetComponentLocation();
	FVector meshVel = MeshComponent->GetComponentVelocity();

	FComponentQueryParams params;
	params.bTraceComplex = false;
	params.bFindInitialOverlaps = false;

	// We only add 10% velocity for the end of the sweep since we need to sweep past the current position for it to work. 
	// However, if we sweep too far into the future we'll generate hits we don't want this frame.
	GetWorld()->ComponentSweepMulti(hitResults, MeshComponent, meshLoc - meshVel, meshLoc+ (0.1f * meshVel), MeshComponent->GetComponentRotation(), params);
	
	AActor* actorToMatch;
	auto pred = [&actorToMatch](FHitResult& hit) 
	{
		return hit.Actor == actorToMatch;
	};

	for (auto& pair : outActorToHit)
	{
		actorToMatch = pair.Key;
		FHitResult* hitResultMatch = hitResults.FindByPredicate(pred); 

		if (hitResultMatch != nullptr)
		{
			pair.Value = *hitResultMatch;
		}
	}
}

void ASword::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bIsSwordFastEnough)
	{
		damageAllOverlappingActors();
	}
}

void ASword::damageAllOverlappingActors()  
{
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors, TSubclassOf<APawn>());
	int32 overlaps = overlappingActors.Num();

	if (overlaps == 0)
	{
		return;
	}

	TArray<TPair<AActor*, FHitResult>> actorToHit;
	
	for (AActor* actor : overlappingActors)
	{
		int32 idx = actorToHit.Add(TPair<AActor*, FHitResult>());
		actorToHit[idx].Key = actor;
	}

	getOverlappingHits(actorToHit);
	actorToHit.RemoveAll([](const auto& pair) 
	{
		return pair.Value.Actor == nullptr;
	});

	bool hasDealtdamage = false;
	AActor* owner = GetOwner();
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
	MeshComponent->SetMaterial(0, material_Dyn);
}

void ASword::tryLaunchCharacter(ACharacter* character) const
{
	check(character);
	UCharacterMovementComponent* charMoveComp = CastChecked<UCharacterMovementComponent>(character->GetMovementComponent());
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
	return bIsSwordFastEnough
		&& GetOwner() != OtherCharacter
		&& UMlgGameplayStatics::CanDealDamageTo(this, OtherCharacter);
}

void ASword::dealDamageTo(ACharacter* OtherCharacter, const FHitResult& HitResult) 
{
	check(OtherCharacter);

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
