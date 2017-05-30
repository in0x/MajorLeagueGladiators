// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuCharacter.h"
#include "Menu/MenuGameMode.h"
#include "CollisionStatics.h"
#include "MenuActionComponent.h"

namespace
{
	const FName NO_COLLISION_PROFILE_NAME("NoCollision");
	const FName AIM_SOCKET_NAME("Aim");
}

AMenuCharacter::AMenuCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MenuSelectionRayCastRange(1000000.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	BaseEyeHeight = 0.0f;
	CrouchedEyeHeight = 0.0f;

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);

	ConstructorHelpers::FObjectFinder<UStaticMesh> viveMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/vive_controller.vive_controller'"));
	if (viveMesh.Succeeded())
	{
		leftMesh->SetStaticMesh(viveMesh.Object);
		rightMesh->SetStaticMesh(viveMesh.Object);
	}

	LeftMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	RightMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	leftMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	rightMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
}

void AMenuCharacter::Tick(float DeltaTimeSeconds)
{
	const FTransform aimSocketTransform = rightMesh->GetSocketTransform(FName("Aim"));

	const FVector start = aimSocketTransform.GetLocation();
	const FVector direction = aimSocketTransform.GetRotation().GetForwardVector();
	const FVector end = start + direction * MenuSelectionRayCastRange;

	DrawDebugLine(GetWorld(), start, end, FColor::Red,false, -1.f, 0, 2.f);
}

void AMenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRightTriggerClicked);
}

void AMenuCharacter::OnRightTriggerClicked()
{
	const FTransform aimSocketTransform = rightMesh->GetSocketTransform(FName("Aim"));

	const FVector start = aimSocketTransform.GetLocation();
	const FVector direction = aimSocketTransform.GetRotation().GetForwardVector();
	const FVector end = start + direction * MenuSelectionRayCastRange;

	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
	};

	TArray<FHitResult> hitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	const UWorld* world = GetWorld();
	world->LineTraceMultiByObjectType(hitResults, start, end, queryTypes, CollisionParams);
	
	for (FHitResult& hitResult : hitResults)
	{
		if (hitResult.Actor.IsValid())
		{
			UMenuActionComponent* menuAction = hitResult.Actor->FindComponentByClass<UMenuActionComponent>();

			if (menuAction)
			{
				menuAction->TriggerMenuAction();
			}
		}
	}
}



