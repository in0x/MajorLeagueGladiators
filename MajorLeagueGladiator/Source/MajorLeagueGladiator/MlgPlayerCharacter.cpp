// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"
#include "MlgPlayerController.h"
#include "WidgetComponent.h"
#include "PlayerHudWidget.h"
#include "TextWidget.h"
#include "DamageReceiverComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/MlgAbilitySet.h"

AMlgPlayerCharacter::AMlgPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Left Grip Motion Controller"))
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Right Grip Motion Controller"))
	)
{
	bUseControllerRotationPitch = true;

	healthComp = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, TEXT("HealthComp"));
	healthComp->SetIsReplicated(true);

	dmgReceiverComp = ObjectInitializer.CreateDefaultSubobject<UDamageReceiverComponent>(this, TEXT("DmgReceiverComp"));

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	bodyMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BodyMesh"));

	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);
	bodyMesh->SetupAttachment(VRReplicatedCamera);

	hudHealth = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HUDHealth"));
	hudHealth->SetupAttachment(leftMesh, FName(TEXT("Touch")));
	
	abilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbilitySystemComponent>(this, TEXT("GameplayTasks"));
	abilitySystemComponent->SetIsReplicated(true);

	abilitySetClass = UMlgAbilitySet::StaticClass();

	GetMovementComponent()->SetIsReplicated(true);
}

void AMlgPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// If no HMD is connected, setup non VR mode.
	if (!g_IsVREnabled())
	{
		pHandMotionController = std::make_unique<HandMotionController>(this);

		this->bUseControllerRotationPitch = true;
		this->bUseControllerRotationRoll = true;
		this->bUseControllerRotationYaw = true;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));
	}	
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("VR MODE"));
		GEngine->HMDDevice->SetBaseOrientation(FQuat::Identity);
	}

	if (healthTriggerClass)
	{
		auto healthTrigger = GetWorld()->SpawnActor<AActor>(healthTriggerClass, FTransform());
		healthTrigger->AttachToComponent(VRReplicatedCamera, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
		auto triggerComp = healthTrigger->GetComponentByClass(UTriggerZoneComponent::StaticClass());

		healthTrigger->SetActorScale3D(FVector(0.3, 0.3, 0.3));

		if (triggerComp)
		{
			CastChecked<UTriggerZoneComponent>(triggerComp)->SetTriggerType(TriggerType::Health);
		}
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("No class set for playercharacter health trigger"))
	}

	auto healthWidget = CastChecked<UPlayerHudWidget>(hudHealth->GetUserWidgetObject());
	healthWidget->OnAttachPlayer(this);

	LandedDelegate.AddDynamic(this, &AMlgPlayerCharacter::OnLand);
}

void AMlgPlayerCharacter::OnLand(const FHitResult& hit)
{
	GetCharacterMovement()->StopMovementImmediately();
}

void AMlgPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMlgPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMlgPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMlgPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMlgPlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed,  this, &AMlgPlayerCharacter::OnRightTriggerClicked);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Released, this, &AMlgPlayerCharacter::OnRightTriggerReleased);

	PlayerInputComponent->BindAction("SideGripButtonLeft", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnSideGripButtonLeft);
	PlayerInputComponent->BindAction("SideGripButtonRight", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnSideGripButtonRight);


	// SUPER IMPORTANT, must be called, when playercontroller status changes (in this case we are sure that we now possess a Controller so its different than before)
	// Though we might move it to a different place
	abilitySystemComponent->RefreshAbilityActorInfo();

	const UMlgAbilitySet* abilitySet = GetOrLoadAbilitySet();
	if (!abilitySet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Set Is null. GiveAbilities has not been called."));
	}
	else
	{
		abilitySet->BindAbilitiesToInput(PlayerInputComponent, abilitySystemComponent);
	}
	
}

void AMlgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	abilitySystemComponent->RefreshAbilityActorInfo();
	if (Role >= ROLE_Authority)
	{
		
		const UMlgAbilitySet* abilitySet = GetOrLoadAbilitySet();
		if(abilitySet)
		{
			abilitySet->GiveAbilities(abilitySystemComponent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability Set Is null. GiveAbilities has not been called."));
		}
		
	} 
}

const UMlgAbilitySet* AMlgPlayerCharacter::GetOrLoadAbilitySet()
{
	if(!cachedAbilitySet)
	{
		cachedAbilitySet = abilitySetClass.LoadSynchronous()->GetDefaultObject<UMlgAbilitySet>();
	}
	
	return cachedAbilitySet;
}

void AMlgPlayerCharacter::BecomeViewTarget(APlayerController* PC) 
{
	Super::BecomeViewTarget(PC);
}

void AMlgPlayerCharacter::MoveForward(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(direction, Value);
}

void AMlgPlayerCharacter::MoveRight(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(direction, Value);
}

void AMlgPlayerCharacter::OnLeftTriggerClicked()
{
	leftHandGrab_Server();
}

void AMlgPlayerCharacter::OnLeftTriggerReleased()
{
	leftHandRelease_Server();
}

void AMlgPlayerCharacter::OnRightTriggerClicked()
{
	rightHandGrab_Server();
}

void AMlgPlayerCharacter::OnRightTriggerReleased()
{
	rightHandRelease_Server();
}

void AMlgPlayerCharacter::OnSideGripButtonLeft()
{
	leftHandDrop_Server();
}

void AMlgPlayerCharacter::OnSideGripButtonRight()
{
	rightHandDrop_Server();
}

UStaticMeshComponent* AMlgPlayerCharacter::GetMotionControllerMesh(EControllerHand Hand)
{
	switch (Hand)
	{
	case EControllerHand::Left:
		return leftMesh;
	case EControllerHand::Right:
		return rightMesh;
	default:
		checkNoEntry();
		return nullptr;
	}
}

UVRControllerComponent* AMlgPlayerCharacter::GetMotionController(EControllerHand Hand)
{
	switch (Hand)
	{
	case EControllerHand::Left:
		return CastChecked<UVRControllerComponent>(LeftMotionController);
	case EControllerHand::Right:
		return CastChecked<UVRControllerComponent>(RightMotionController);
	default:
		checkNoEntry();
		return nullptr;
	}
}

// Left hand.
bool AMlgPlayerCharacter::leftHandGrab_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::leftHandGrab_Server_Implementation()
{
	UVRControllerComponent* leftController = CastChecked<UVRControllerComponent>(LeftMotionController);
	leftController->UseGrippedActors();
	leftController->GrabNearestActor();
}

bool AMlgPlayerCharacter::leftHandRelease_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::leftHandRelease_Server_Implementation()
{
	UVRControllerComponent* leftController = CastChecked<UVRControllerComponent>(LeftMotionController);
	leftController->EndUseGrippedActors();
	leftController->DropNonInteractGrips();
}

bool AMlgPlayerCharacter::leftHandDrop_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::leftHandDrop_Server_Implementation()
{
	UVRControllerComponent* leftController = CastChecked<UVRControllerComponent>(LeftMotionController);
	leftController->EndUseGrippedActors();
	leftController->DropAllGrips();
}

// Right hand.
bool AMlgPlayerCharacter::rightHandGrab_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::rightHandGrab_Server_Implementation()
{
	UVRControllerComponent* rightController = CastChecked<UVRControllerComponent>(RightMotionController);
	rightController->UseGrippedActors();
	rightController->GrabNearestActor();
}

bool AMlgPlayerCharacter::rightHandRelease_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::rightHandRelease_Server_Implementation()
{
	UVRControllerComponent* rightController = CastChecked<UVRControllerComponent>(RightMotionController);
	rightController->EndUseGrippedActors();
	rightController->DropNonInteractGrips();
}

bool AMlgPlayerCharacter::rightHandDrop_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::rightHandDrop_Server_Implementation()
{
	UVRControllerComponent* rightController = CastChecked<UVRControllerComponent>(RightMotionController);
	rightController->EndUseGrippedActors();
	rightController->DropAllGrips();
}

void AMlgPlayerCharacter::EnableActorCollison_NetMulticast_Implementation(bool bNewActorEnableCollision)
{
	SetActorEnableCollision(bNewActorEnableCollision);
}