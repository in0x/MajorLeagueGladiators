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
#include "MlgGrippableStaticMeshActor.h"
#include "../Plugins/Runtime/Steam/SteamVR/Source/SteamVR/Classes/SteamVRChaperoneComponent.h"
#include "TriggerZoneComponent.h"

namespace 
{
	const char* PAWN_COLLISION_PROFILE_NAME = "Pawn";
	const char* NO_COLLISION_PROFILE_NAME = "NoCollision";
	const char* VR_CAPSULE_COLLISION_NAME = "VrCapsule";
	const FVector INVALID_TARGET_LOCATION = FVector(0,0, 9'999'999);
}

AMlgPlayerCharacter::AMlgPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Left Grip Motion Controller"))
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Right Grip Motion Controller"))
	)
{
	BaseEyeHeight = 0.0f;
	CrouchedEyeHeight = 0.0f;
	bUseControllerRotationYaw = false;
	PrimaryActorTick.bCanEverTick = true;

	CastChecked<UPrimitiveComponent>(RootComponent)->SetCollisionProfileName(PAWN_COLLISION_PROFILE_NAME);

	healthComp = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, TEXT("HealthComp"));
	healthComp->SetIsReplicated(true);	

	dmgReceiverComp = ObjectInitializer.CreateDefaultSubobject<UDamageReceiverComponent>(this, TEXT("DmgReceiverComp"));

	leftMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("RightMesh"));
	bodyMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BodyMesh"));
	bodyMesh2 = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BodyMesh2"));
	
	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);

	chaperone = ObjectInitializer.CreateDefaultSubobject<USteamVRChaperoneComponent>(this, TEXT("Chaperone"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> multiMesh(TEXT("SkeletalMesh'/Game/MVRCFPS_Assets/MultiTool/Gun_handle.Gun_handle'"));
	if (multiMesh.Succeeded())
	{
		leftMesh->SetSkeletalMesh(multiMesh.Object);
		rightMesh->SetSkeletalMesh(multiMesh.Object);
	}
	
	GetCapsuleComponent()->SetCollisionProfileName(VR_CAPSULE_COLLISION_NAME);

	LeftMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	RightMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	leftMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	rightMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);

	bodyMesh->SetupAttachment(VRReplicatedCamera);
	bodyMesh->SetOwnerNoSee(true);
	bodyMesh->SetCollisionProfileName(PAWN_COLLISION_PROFILE_NAME);

	bodyMesh2->SetupAttachment(VRReplicatedCamera);
	bodyMesh2->SetOwnerNoSee(true);
	bodyMesh2->SetCollisionProfileName(PAWN_COLLISION_PROFILE_NAME);
	bodyMesh2->SetRelativeLocation({ 0,0,0 });
	bodyMesh2->bAbsoluteRotation = true;
	bodyMesh2->bAbsoluteScale = true;
	bodyMesh2->bAbsoluteLocation = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> bodyMeshAsset(TEXT("StaticMesh'/Game/MVRCFPS_Assets/PlayerCharacters/tank_body.tank_body'"));
	bodyMesh2->SetStaticMesh(bodyMeshAsset.Object);

	BodyOffsetFromHead = { 0,0,-190 };
	

	hudHealth = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HUDHealth"));
	hudHealth->SetupAttachment(leftMesh, FName(TEXT("Touch")));
	hudHealth->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMesh(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	healthTriggerZone = ObjectInitializer.CreateDefaultSubobject<UTriggerZoneComponent>(this, TEXT("healthTriggerZone"));
	healthTriggerZone->SetupAttachment(VRReplicatedCamera);
	healthTriggerZone->SetTriggerType(TriggerType::Health);
	healthTriggerZone->SetRelativeScale3D({ 0.2, 0.2, 0.2 });
	healthTriggerZone->SetRelativeLocation({ 0, 0, -20});
	healthTriggerZone->SetStaticMesh(sphereMesh.Object);

	auto classString = TEXT("WidgetBlueprint'/Game/BluePrints/PlayerHudBP.PlayerHudBP'");
	classString = TEXT("/Game/BluePrints/PlayerHudBP");
	static const ConstructorHelpers::FClassFinder<UUserWidget> healthWidgetClass(classString);
	hudHealth->SetWidgetClass(healthWidgetClass.Class);
	hudHealth->SetDrawSize({ 400, 100 });
	hudHealth->SetTwoSided(true);
	
	abilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbilitySystemComponent>(this, TEXT("GameplayTasks"));
	abilitySystemComponent->SetIsReplicated(true);

	abilitySetClass = UMlgAbilitySet::StaticClass();

	GetMovementComponent()->SetIsReplicated(true);
}

void AMlgPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	pChaperoneBounds = std::make_unique<ChaperoneBounds>(chaperone);

	if (g_IsVREnabled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("VR MODE"));
		GEngine->HMDDevice->SetBaseOrientation(FQuat::Identity);
	}
	else
	{
		pHandMotionController = std::make_unique<HandMotionController>(this);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));
#if 1 //If this is on you can move with the mouse, however it also causes the sliding bug
		bUseControllerRotationPitch = true;
		bUseControllerRotationRoll = true;
		bUseControllerRotationYaw = true;
#endif
	}
	
	auto healthWidget = CastChecked<UPlayerHudWidget>(hudHealth->GetUserWidgetObject());
	healthComp->HealthChangedDelegate.AddDynamic(healthWidget, &UPlayerHudWidget::SetCurrentPercentage);
	healthWidget->SetCurrentPercentage(healthComp->GetCurrentHealthPercentage(), 1.0f);
	
	LandedDelegate.AddDynamic(this, &AMlgPlayerCharacter::OnLand);
	
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}

void AMlgPlayerCharacter::OnLand(const FHitResult& hit)
{
	StopMovementImmediately_NetMulticast();
}

FVector AMlgPlayerCharacter::GetProjectedLocation() const
{
	return abilityMoveTargetLocation == INVALID_TARGET_LOCATION ? GetActorLocation() : abilityMoveTargetLocation;
}

void AMlgPlayerCharacter::SetAbilityMoveTargetLocation(FVector Location)
{
	abilityMoveTargetLocation = Location;
	OnAbilityMoveTargetLocationSet.Broadcast(abilityMoveTargetLocation);
}

void AMlgPlayerCharacter::InvalidateAbilityMoveTargetLocation()
{
	abilityMoveTargetLocation = INVALID_TARGET_LOCATION;
}

void AMlgPlayerCharacter::Tick(float DelataTime)
{
	Super::Tick(DelataTime);

	FTransform cameraTrans = VRReplicatedCamera->GetComponentTransform();

	FRotator rot(0, cameraTrans.Rotator().Yaw, 0);

	FRotator bodyRotation = bodyMesh2->GetComponentRotation();
	
	bodyMesh2->SetWorldTransform(FTransform(rot, BodyOffsetFromHead + cameraTrans.GetLocation()));
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
		UE_LOG(DebugLog, Warning, TEXT("Ability Set Is null. GiveAbilities has not been called."));
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
			UE_LOG(DebugLog, Warning, TEXT("Ability Set Is null. GiveAbilities has not been called."));
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

	FaceRotation(Controller->GetControlRotation());
}

void AMlgPlayerCharacter::SpawnWeapon()
{
	check(HasAuthority());
	if (startWeaponClass.Get())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		attachedWeapon = GetWorld()->SpawnActor<AMlgGrippableStaticMeshActor>(
			startWeaponClass.Get(), GetTransform(), spawnParams);

		OnAttachedWeaponSet();	
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Spawning Start weapon was not set"));
	}
}

void AMlgPlayerCharacter::OnRep_AttachedWeapon()
{
	OnAttachedWeaponSet();
}
void AMlgPlayerCharacter::OnAttachedWeaponSet()
{
	// We don't replicate the movement, as it is set by the motion controllers.
	// Thus it would be unnecessary and it also fixes a bug :)
	// However the values of the object will still be replicated and RPC can be used
	attachedWeapon->SetReplicateMovement(false);

	GetMotionController(EControllerHand::Right)->GripActor(
		attachedWeapon, GetTransform(), true, TEXT("VRGripP1"),
		EGripCollisionType::InteractiveCollisionWithPhysics,
		EGripLateUpdateSettings::NotWhenCollidingOrDoubleGripping,
		EGripMovementReplicationSettings::LocalOnly_Not_Replicated, 1500000, 2000);
}

AMlgGrippableStaticMeshActor* AMlgPlayerCharacter::GetAttachedWeapon()
{
	return attachedWeapon;
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

USkeletalMeshComponent* AMlgPlayerCharacter::GetMotionControllerMesh(EControllerHand Hand)
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

FVector AMlgPlayerCharacter::AMlgPlayerCharacter::CalcFeetPosition() const
{
	const UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	const FVector CapsuleLocation = capsuleComponent->GetComponentLocation();	
	const FVector FeetOffset = capsuleComponent->GetUpVector() * capsuleComponent->GetScaledCapsuleHalfHeight();	
	return CapsuleLocation - FeetOffset;
}

// Left hand.
bool AMlgPlayerCharacter::leftHandGrab_Server_Validate()
{
	return true;
}

void AMlgPlayerCharacter::leftHandGrab_Server_Implementation()
{
	UVRControllerComponent* leftController = CastChecked<UVRControllerComponent>(LeftMotionController);
	if (leftController->HasGrip())
	{
		leftController->UseGrippedActors();
	}
	else
	{
		leftController->GrabNearestActor();
	}
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
	if (rightController->HasGrip())
	{
		rightController->UseGrippedActors();
	}
	else
	{
		rightController->GrabNearestActor();
	}
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

void AMlgPlayerCharacter::LaunchCharacter_NetMulticast_Implementation(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void AMlgPlayerCharacter::StopMovementImmediately_NetMulticast_Implementation()
{
	GetMovementComponent()->StopMovementImmediately();
}

void AMlgPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMlgPlayerCharacter, attachedWeapon);
}

