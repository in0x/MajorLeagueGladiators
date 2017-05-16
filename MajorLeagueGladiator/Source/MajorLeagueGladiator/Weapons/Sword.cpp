// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

#include "MlgGameplayStatics.h"
#include "MlgPlayerController.h"
#include "CollisionStatics.h"
#include "DamageTypes/SwordDamage.h"

//TODO (Flo): FIX multi hit issue

namespace
{
	const FName MELEE_WEAPON_COLLISION_PROFILE_NAME("MeleeWeapon");
	const FName SWORD_MATERIAL_GLOW_COLOR_PARAMETER_NAME("Glow Color");
	const FName SWORD_MATERIAL_GLOW_STRENGTH_PARAMETER_NAME("Glow Strength");
}

ASword::ASword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, oldSwingSpeed(FVector::ZeroVector)
	, threshholdDoDamageSquared(2)
	, slashVelocityLearnRate(0.1f)
	, bIsSwordFastEnough(false)
	, damageAppliedOnHit(40.f)
	, damageType(USwordDamage::StaticClass())
	, materialInstance(nullptr)
	, glowStrengthMultiplier(50.f)
	, minGlowStrength(6.f)
	, maxGlowStrength(250.f)
	, damageSwordColor(1, 0, 0)
	, originalSwordColor(0.002732f, 1.f, 0.904661f)
{
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(MELEE_WEAPON_COLLISION_PROFILE_NAME);
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SwordStaticMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/MultiTool/sword/sword_blade_3.sword_blade_3'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (SwordStaticMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(SwordStaticMesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Static Mesh for Sword not found."));
	}

	ConstructorHelpers::FObjectFinder<USoundCue> swordSwingCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/SwordSwing_Cue.SwordSwing_Cue'"));
	swordSwingCue = swordSwingCueFinder.Object;
}

void ASword::BeginPlay()
{
	Super::BeginPlay();
	materialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (materialInstance == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("ASword::BeginPlay: Material Missing"));
	}
	updateMaterialColor(originalSwordColor);
}

void ASword::updateMaterialIntensity(const float intensity)
{
	if (materialInstance == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::updateMaterial: Material Instance Missing"));
		return;
	}
	materialInstance->SetScalarParameterValue(SWORD_MATERIAL_GLOW_STRENGTH_PARAMETER_NAME, intensity);
}

void ASword::updateMaterialColor(const FLinearColor& color)
{
	if (materialInstance == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("ABasePack::updateMaterial: Material Instance Missing"));
		return;
	}
	materialInstance->SetVectorParameterValue(SWORD_MATERIAL_GLOW_COLOR_PARAMETER_NAME, color);
}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateVelocity(DeltaTime);

	const FVector relativeSwordVelocity = calcRelativeVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashVelocityLearnRate) + (DeltaTime * relativeSwordVelocity) * slashVelocityLearnRate;
	const float currentVelocitySquared = oldSwingSpeed.SizeSquared();
	bool newIsSwordFastEnough = currentVelocitySquared > threshholdDoDamageSquared || bIsAlwaysFastEnough;
	
	float glowIntensity = currentVelocitySquared * glowStrengthMultiplier;
	updateMaterialIntensity(FMath::Clamp(glowIntensity, minGlowStrength, maxGlowStrength));

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

void ASword::SetIsAlwaysFastEnough(bool IsAlwaysFastEnough)
{
	if (!HasAuthority())
	{
		UE_LOG(DebugLog, Warning, TEXT("SetAlwaysFastEnough should only be called on server"));
	}

	bIsAlwaysFastEnough = IsAlwaysFastEnough;
}

bool ASword::GetIsAlawaysFastEnough() const
{
	return bIsAlwaysFastEnough;
}

void ASword::onStartSlash()
{
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		swordSwingCue,
		GetActorLocation());

	updateMaterialColor(damageSwordColor);
}

void ASword::onEndSlash()
{
	updateMaterialColor(originalSwordColor);
}

void ASword::getOverlappingHits(TArray<TPair<AActor*, FHitResult>>& outActorToHit) const
{
	TArray<FHitResult> hitResults;

	FVector actorLoc = GetActorLocation();
	FVector actorVel = currentVelocity;

	FComponentQueryParams params;
	params.bTraceComplex = false;
	params.bFindInitialOverlaps = false;

	// We only add 10% velocity for the end of the sweep since we need to sweep past the current position for it to work. 
	// However, if we sweep too far into the future we'll generate hits we don't want this frame.
	GetWorld()->ComponentSweepMulti(hitResults, MeshComponent, actorLoc - actorVel, actorLoc+ (0.1f * actorVel), MeshComponent->GetComponentRotation(), params);
	
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

void ASword::UpdateVelocity(float DeltaTime)
{
	const FVector currentLocation = GetActorLocation();
	const FVector moveDelta = currentLocation - lastLocation;
	currentVelocity = moveDelta / DeltaTime;
	lastLocation = currentLocation;
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

	return currentVelocity - ownerVelocity;
}

void ASword::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASword, bIsAlwaysFastEnough);
}
