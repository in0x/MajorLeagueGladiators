// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuCharacter.h"
#include "Menu/MenuGameMode.h"
#include "CollisionStatics.h"
#include "MenuActionComponent.h"
#include "MlgGameInstance.h"

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

	pointerMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PointerMesh"));
	pointerMesh->SetMobility(EComponentMobility::Movable);
	pointerMesh->SetupAttachment(rightMesh, AIM_SOCKET_NAME);

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
		PlayerInputComponent->BindAction("JoinFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::JoinFirstFriendInList);
		PlayerInputComponent->BindAction("InviteFriend", EInputEvent::IE_Pressed, this, &AMenuCharacter::InviteFirstPlayerInFriendslist);


	}
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

void AMenuCharacter::JoinFirstFriendInList()
{
	UGameInstance* gameInstance = GetGameInstance();
	UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);

	mlgGameInstance->ReadFriendList();
	mlgGameInstance->JoinFirstAvailableFriend();
}

void AMenuCharacter::InviteFirstPlayerInFriendslist()
{
	UGameInstance* gameInstance = GetGameInstance();
	UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);

	// Currently will display as debug Draw
	mlgGameInstance->ReadFriendList();
	mlgGameInstance->InviteFirstAvailableFriend();
}

