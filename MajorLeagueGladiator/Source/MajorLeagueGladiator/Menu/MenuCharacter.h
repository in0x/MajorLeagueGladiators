// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "MenuActionComponent.h"
#include "HandMotionController.h"
#include "MenuCharacter.generated.h"

class UWidgetInteractionComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMenuCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	AMenuCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	FMenuActionDelegate OnMenuActionTriggered;

private:
	void OnRightTriggerClicked();
	void OnRightTriggerReleased();
	void MoveForward(float Value);
	void MoveRight(float Value);

	void OnHostGamePressed();
	void OnJoinGamePressed();
	void OnMeleeTutPressed();
	void OnRangeTutPressed();

	void OnShowFriends();
	void OnJoinFirstFriendInList();
	void OnInviteFirstPlayerInFriendslist();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* rightMesh;
	
	UPROPERTY(EditAnywhere)
	UWidgetInteractionComponent* widgetInteraction;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* menuWidgetComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* menuPointerMesh;

	UPROPERTY(EditAnywhere)
	float MenuSelectionRayCastRange;

	std::unique_ptr<HandMotionController> pHandMotionController;
};
