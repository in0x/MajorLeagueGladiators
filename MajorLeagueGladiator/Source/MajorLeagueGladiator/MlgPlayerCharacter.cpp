// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "TeleportComponent.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"
#include "MlgPlayerController.h"
#include "WidgetComponent.h"
#include "PlayerHudWidget.h"
#include "TextWidget.h"
#include "DamageReceiverComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySet.h"
#include "AbilitySystemExample/TargetingGameplayAbility.h"

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

	leftGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("LeftGrabSphere"));
	rightGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RightGrabSphere"));

	leftGrabSphere->SetupAttachment(LeftMotionController);
	rightGrabSphere->SetupAttachment(RightMotionController);

	hudHealth = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HUDHealth"));
	hudHealth->SetupAttachment(VRReplicatedCamera);
	
	hudTeleportCD = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HudTeleportCD"));
	hudTeleportCD->SetupAttachment(leftMesh, FName(TEXT("Touch"), EFindName::FNAME_Find));
	
	teleportComp = ObjectInitializer.CreateDefaultSubobject<UTeleportComponent>(this, TEXT("TeleportComp"));
	teleportComp->Disable();

	abilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbilitySystemComponent>(this, TEXT("GameplayTasks"));
	abilitySystemComponent->SetIsReplicated(true);

	//Change to blueprint to add own abilities
	//gameplayAbilitySet = ObjectInitializer.CreateDefaultSubobject<UGameplayAbilitySet>(this, TEXT("GameplayAbilitySet"));

	//// Serves as an example can be removed without consequences (unless this is actually used)
	//// We might want to have a different way on how to add abilities in the future
	//FGameplayAbilityBindInfo mockAbility;
	//mockAbility.Command = EGameplayAbilityInputBinds::Ability1;
	//mockAbility.GameplayAbilityClass = UTargetingGameplayAbility::StaticClass();
	//gameplayAbilitySet->Abilities.Add(mockAbility);
}

void AMlgPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// SUPER IMPORTANT, must be called, when playercontroller status changes (in this case we are sure that we now possess a Controller so its different than before)
	// Though we might move it to a different place
	abilitySystemComponent->RefreshAbilityActorInfo();

	// If no HMD is connected, setup non VR mode.
	if (!GEngine->HMDDevice.IsValid() || !GEngine->HMDDevice->IsHMDConnected()) 
	{
		pHandMotionController = std::make_unique<HandMotionController>(this);

		// Set Capsule collider dimensions so that the player has an actual height in non VR mode.
		// Having this set in VR Mode causes the HMD to be offset too far from the floor.
		/*auto capsule = CastChecked<UCapsuleComponent>(GetRootComponent());
		capsule->SetCapsuleRadius(20.f);
		capsule->SetCapsuleHalfHeight(96.f);*/

		VRSceneComponent->SetRelativeLocation(FVector(0, 0, 0));
		//VRSceneComponent->SetWorldLocation(capsule->GetComponentLocation() + FVector(0.f, 0.f, 96.f));

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));
	}	
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("VR MODE"));
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

	teleportComp->OnCooldownChange.AddLambda([textWidget = CastChecked<UTextWidget>(hudTeleportCD->GetUserWidgetObject())](float CurrentCD)
	{
		textWidget->SetText(FString::FromInt(static_cast<int>(FMath::RoundFromZero(CurrentCD))));
	});
	//if (Role >= ROLE_Authority)
	//{
	//	gameplayAbilitySet->GiveAbilities(abilitySystemComponent);
	//}
}

void AMlgPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMlgPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMlgPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMlgPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMlgPlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Pressed,  this, &AMlgPlayerCharacter::OnLeftTriggerClicked);
	PlayerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Released, this, &AMlgPlayerCharacter::OnLeftTriggerReleased);

	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed,  this, &AMlgPlayerCharacter::OnRightTriggerClicked);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Released, this, &AMlgPlayerCharacter::OnRightTriggerReleased);

	PlayerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnTeleportPressedLeft);
	PlayerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnTeleportPressedRight);

	PlayerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Released, this, &AMlgPlayerCharacter::OnTeleportReleased);
	PlayerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Released, this, &AMlgPlayerCharacter::OnTeleportReleased);

	PlayerInputComponent->BindAction("SideGripButtonLeft", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnSideGripButtonLeft);
	PlayerInputComponent->BindAction("SideGripButtonRight", EInputEvent::IE_Pressed, this, &AMlgPlayerCharacter::OnSideGripButtonRight);

	//Binds skill confirmation and canceling, but not skills themselves
	//abilitySystemComponent->BindToInputComponent(PlayerInputComponent);

	////Bind each Skill induvidually
	//for (const FGameplayAbilityBindInfo& BindInfo : gameplayAbilitySet->Abilities)
	//{
	//	if (!BindInfo.GameplayAbilityClass)
	//	{
	//		UE_LOG(LogAbilitySystem, Warning, TEXT("GameplayAbilityClass %d not set"), (int32)BindInfo.Command);
	//		continue;
	//	}

	//	FGameplayAbilitySpec spec(
	//		BindInfo.GameplayAbilityClass->GetDefaultObject<UGameplayAbility>(), 1, (int32)BindInfo.Command);
	//	
	//	int32 AbilityID = (int32)BindInfo.Command;

	//	FGameplayAbiliyInputBinds inputBinds(
	//		FString::Printf(TEXT("ConfirmAbility%d"), AbilityID),
	//		FString::Printf(TEXT("CancelAbility%d"), AbilityID),
	//		"EGameplayAbilityInputBinds",
	//		AbilityID, 
	//		AbilityID
	//		);

	//	abilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, inputBinds);		
	//}

	

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

void AMlgPlayerCharacter::OnTeleportPressedLeft()
{
	teleportComp->Enable(LeftMotionController);
}

void AMlgPlayerCharacter::OnTeleportPressedRight()
{
	teleportComp->Enable(RightMotionController);
}

void AMlgPlayerCharacter::OnSideGripButtonLeft()
{
	leftHandDrop_Server();
}

void AMlgPlayerCharacter::OnSideGripButtonRight()
{
	rightHandDrop_Server();
}

void AMlgPlayerCharacter::OnTeleportReleased()
{
	auto result = teleportComp->GetTeleportResult();
	teleportComp->Disable();

	if (result.ShouldTeleport)
	{
		if (Role >= ROLE_Authority)
		{
			performTeleport_NetMulticast(result.Position, GetActorRotation());
		}
		else
		{
			requestTeleport_Server(result.Position, GetActorRotation());
		}		
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
	leftController->GrabNearestActor(*leftGrabSphere);
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
	rightController->GrabNearestActor(*rightGrabSphere);
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

bool AMlgPlayerCharacter::requestTeleport_Server_Validate(FVector Location, FRotator Rotation)
{
	//you could check if teleport is realistic/possible to prevent cheats
	return true;
}

void AMlgPlayerCharacter::requestTeleport_Server_Implementation(FVector Location, FRotator Rotation)
{
	performTeleport_NetMulticast(Location, Rotation);
}

void AMlgPlayerCharacter::performTeleport_NetMulticast_Implementation(FVector Location, FRotator Rotation)
{
	TeleportTo(Location, Rotation, false, true);
}