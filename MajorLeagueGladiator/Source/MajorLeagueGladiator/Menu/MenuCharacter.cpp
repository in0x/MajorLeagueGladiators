// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuCharacter.h"
#include "Menu/MenuGameMode.h"
#include "CollisionStatics.h"
#include "MenuActionComponent.h"
#include "MlgGameInstance.h"
#include "WidgetInteractionComponent.h"

namespace
{
	const FName NO_COLLISION_PROFILE_NAME("NoCollision");
	const FName AIM_SOCKET_NAME("Aim");
}

AMenuCharacter::AMenuCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MenuSelectionRayCastRange(1000000.f)
{
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

	// TODO(Phil): Pull out pointer mesh, hook widgetinteraction to input, build menuwidget, add it to character
	widgetInteraction = ObjectInitializer.CreateDefaultSubobject<UWidgetInteractionComponent>(this, TEXT("WidgetInteraction"));
	widgetInteraction->SetupAttachment(rightMesh, FName(TEXT("Touch")));
	widgetInteraction->bShowDebug = true;

	menuWidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("MenuWidgetComponent"));
	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuWidget(TEXT("/Game/BluePrints/Menu/MainMenuWidget"));
	menuWidgetComponent->SetWidgetClass(mainMenuWidget.Class);
	menuWidgetComponent->SetupAttachment(leftMesh, FName(TEXT("Touch")));

#if 1 //If this is on you can move with the mouse, however it also causes the sliding bug
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;
#endif
}

void AMenuCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	}
}

void AMenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRightTriggerClicked);

	if (!g_IsVREnabled())
	{
		PlayerInputComponent->BindAction("HostGamePressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnHostGamePressed);
		PlayerInputComponent->BindAction("JoinGamePressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnJoinGamePressed);
		PlayerInputComponent->BindAction("MeleeTutPressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnMeleeTutPressed);
		PlayerInputComponent->BindAction("RangeTutPressed", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRangeTutPressed);

		PlayerInputComponent->BindAction("ShowFriends", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnShowFriends);
		PlayerInputComponent->BindAction("JoinFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnJoinFirstFriendInList);
		PlayerInputComponent->BindAction("InviteFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnInviteFirstPlayerInFriendslist);
	}
}

void AMenuCharacter::OnRightTriggerClicked()
{
	if (widgetInteraction->IsOverInteractableWidget())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Over Interactive Widget"));
	}
	
	widgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
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

