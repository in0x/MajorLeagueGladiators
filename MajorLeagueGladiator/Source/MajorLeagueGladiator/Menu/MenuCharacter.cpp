// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuCharacter.h"
#include "Menu/MenuGameMode.h"
#include "CollisionStatics.h"
#include "MenuActionComponent.h"
#include "MlgGameInstance.h"
#include "WidgetInteractionComponent.h"
#include "MenuUtilities.h"

namespace
{
	const FName NO_COLLISION_PROFILE_NAME("NoCollision");
	const FName AIM_SOCKET_NAME("Aim");
	const FName ATTACHMENT_SOCKET_NAME("Attachment");
}

AMenuCharacter::AMenuCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MenuSelectionRayCastRange(1000000.f)
{
	BaseEyeHeight = 0.0f;
	CrouchedEyeHeight = 0.0f;

	leftViveMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightViveMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	leftViveMesh->SetupAttachment(LeftMotionController);
	rightViveMesh->SetupAttachment(RightMotionController);

	ConstructorHelpers::FObjectFinder<UStaticMesh> viveMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/vive_controller.vive_controller'"));
	if (viveMesh.Succeeded())
	{
		leftViveMesh->SetStaticMesh(viveMesh.Object);
		rightViveMesh->SetStaticMesh(viveMesh.Object);
	}

	LeftMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	RightMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	leftViveMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	rightViveMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);

	widgetInteraction = ObjectInitializer.CreateDefaultSubobject<UWidgetInteractionComponent>(this, TEXT("WidgetInteraction"));
	widgetInteraction->bShowDebug = true;

	menuWidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("MenuWidgetComponent"));
	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuWidget(TEXT("/Game/BluePrints/Menu/MainMenuWidget"));
	menuWidgetComponent->SetWidgetClass(mainMenuWidget.Class);
	menuWidgetComponent->SetTwoSided(true);


	//ConstructorHelpers::FObjectFinder<UStaticMesh> pointerMeshLoader(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> pointerMeshLoader(TEXT("StaticMesh'/Game/MVRCFPS_Assets/pointer.pointer'"));
	
	menuPointerMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MenuPointerMeshComponent"));
	menuPointerMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	menuPointerMesh->SetStaticMesh(pointerMeshLoader.Object);

	MenuUtilities::AttachMenu(leftViveMesh, rightViveMesh, menuWidgetComponent, widgetInteraction, menuPointerMesh, true);
		
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> OcculusControllerLeftMeshLoader(TEXT("StaticMesh'/Game/MVRCFPS_Assets/OculusControllerMesh_Left.OculusControllerMesh_Left'"));

		leftOculusMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftOculus"));
		rightOculusMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightOculus"));

		leftOculusMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
		rightOculusMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);

		leftOculusMesh->SetupAttachment(LeftMotionController);
		rightOculusMesh->SetupAttachment(RightMotionController);

		leftOculusMesh->SetStaticMesh(OcculusControllerLeftMeshLoader.Object);
		rightOculusMesh->SetStaticMesh(OcculusControllerLeftMeshLoader.Object);

		// Flip Mesh
		rightOculusMesh->SetRelativeScale3D(FVector(1, -1, 1));

		leftOculusMesh->SetHiddenInGame(true);
		rightOculusMesh->SetHiddenInGame(true);
	}



#if 1 //If this is on you can move with the mouse, however it also causes the sliding bug
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;
#endif
}

void AMenuCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->IsHeadTrackingAllowed())
	{
		GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	// TODO: Delete the following line and this commet if I left it in by accident!
	AdjustForOculus();

	if (g_IsVREnabled() && GEngine->XRSystem->GetHMDDevice()->GetHMDDeviceType() == EHMDDeviceType::DT_OculusRift)
	{
		AdjustForOculus();
	}

	if (!g_IsVREnabled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'J\' -> Join first Friend"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'I\' -> Invite first Friend"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'L\' -> Refresh friend List"));

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'4\' -> Ranged Tutorial"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'3\' -> Melee Tutorial"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'2\' -> Join Game"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("\'1\' -> Host Game"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Non-VR Menu Bindings:"));

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));

		bUseControllerRotationPitch = true;
		bUseControllerRotationRoll = true;
		bUseControllerRotationYaw = true;

		pHandMotionController = std::make_unique<HandMotionController>(this);
		pHandMotionController->SetCustomRotation(FRotator::MakeFromEuler(FVector(0, 0, 0)));
	}
}

void AMenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRightTriggerClicked);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Released, this, &AMenuCharacter::OnRightTriggerReleased);

	if (!g_IsVREnabled())
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AMenuCharacter::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AMenuCharacter::MoveRight);

		PlayerInputComponent->BindAxis("Turn", this, &AMenuCharacter::AddControllerYawInput);
		PlayerInputComponent->BindAxis("LookUp", this, &AMenuCharacter::AddControllerPitchInput);

		PlayerInputComponent->BindAction("HostGamePressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnHostGamePressed);
		PlayerInputComponent->BindAction("JoinGamePressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnJoinGamePressed);
		PlayerInputComponent->BindAction("MeleeTutPressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnMeleeTutPressed);
		PlayerInputComponent->BindAction("RangeTutPressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRangeTutPressed);

		PlayerInputComponent->BindAction("ShowFriends", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnShowFriends);
		PlayerInputComponent->BindAction("JoinFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnJoinFirstFriendInList);
		PlayerInputComponent->BindAction("InviteFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnInviteFirstPlayerInFriendslist);
	}
}

void AMenuCharacter::AdjustForOculus()
{
	leftViveMesh->SetHiddenInGame(true);
	rightViveMesh->SetHiddenInGame(true);

	leftOculusMesh->SetHiddenInGame(false);
	rightOculusMesh->SetHiddenInGame(false);

	MenuUtilities::AttachMenu(leftOculusMesh, rightOculusMesh, menuWidgetComponent, widgetInteraction, menuPointerMesh, false);
}

void AMenuCharacter::MoveForward(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(direction, Value);
}

void AMenuCharacter::MoveRight(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(direction, Value);
}

void AMenuCharacter::OnRightTriggerClicked()
{
	if (widgetInteraction->IsOverInteractableWidget())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Over Interactive Widget"));
	}
	
	widgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
}

void AMenuCharacter::OnRightTriggerReleased()
{
	widgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AMenuCharacter::OnHostGamePressed()
{
	OnMenuActionTriggered.Broadcast(EMenuAction::HostGame);
}

void AMenuCharacter::OnJoinGamePressed()
{
	OnMenuActionTriggered.Broadcast(EMenuAction::JoinGame);
}

void AMenuCharacter::OnMeleeTutPressed()
{
	OnMenuActionTriggered.Broadcast(EMenuAction::StartMeleeTutorial);
}

void AMenuCharacter::OnRangeTutPressed()
{
	OnMenuActionTriggered.Broadcast(EMenuAction::StartRangedTutorial);
}

void AMenuCharacter::OnShowFriends()
{
	UGameInstance* gameInstance = GetGameInstance();
	UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);

	// Currently will display as debug Draw
	mlgGameInstance->ReadFriendList();
}

void AMenuCharacter::OnJoinFirstFriendInList()
{
	UGameInstance* gameInstance = GetGameInstance();
	UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);

	mlgGameInstance->JoinFirstAvailableFriend();
}
void AMenuCharacter::OnInviteFirstPlayerInFriendslist()
{
	UGameInstance* gameInstance = GetGameInstance();
	UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);

	mlgGameInstance->InviteFirstAvailableFriend();
}

