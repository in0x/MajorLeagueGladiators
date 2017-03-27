// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

#include "MlgGameplayStatics.h"
#include "MlgPlayerController.h"

namespace
{
	const char* MELEE_WEAPON_COLLISION_PROFILE_NAME = "MeleeWeapon";
}

//TODO: FIX multi hit issue

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

	FVector newSwordState = GetVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashVelocityLearnRate) + (DeltaTime * newSwordState) * slashVelocityLearnRate;

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

void ASword::damageAllOverlappingActors()
{
	TArray <AActor*> overlappingActors;
	AActor* owner = GetOwner();
	owner->GetOverlappingActors(overlappingActors);

	int32 overlaps = overlappingActors.Num();

	bool hasDealtdamage = false;
	for (AActor* actor : overlappingActors)
	{
		if (UMlgGameplayStatics::CanDealDamageTo(owner, actor))
		{
			DealDamageTo(CastChecked<ACharacter>(actor));
			hasDealtdamage = true;
		}
	}

	if (hasDealtdamage)
	{
		doRumbleRight();
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
	if (otherCharacter && CanDealDamageTo(otherCharacter))
	{
		DealDamageTo(otherCharacter);
		doRumbleRight();
	}
}


void ASword::LaunchCharacterInSwingDirection(ACharacter* character) const
{
	check(character);
	UCharacterMovementComponent* charMoveComp = 
		CastChecked<UCharacterMovementComponent>(character->GetMovementComponent());
	EMovementMode moveMode = charMoveComp->MovementMode;

	if (moveMode == MOVE_Falling)
	{
		charMoveComp->StopMovementImmediately();
		character->LaunchCharacter(GetVelocity() * 300, true, true);
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

bool ASword::CanDealDamageTo(const ACharacter* OtherCharacter) const
{
	check(OtherCharacter)
	return isSwordFastEnough
		&& GetOwner() != OtherCharacter
		&& UMlgGameplayStatics::CanDealDamageTo(this, OtherCharacter);
}

void ASword::DealDamageTo(ACharacter* OtherCharacter)
{
	check(OtherCharacter)
	UGameplayStatics::ApplyDamage(OtherCharacter, damageAppliedOnHit,
		GetInstigatorController(), this, damageType);
	LaunchCharacterInSwingDirection(OtherCharacter);
}
