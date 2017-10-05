// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "MenuActionComponent.h"
#include "MenuCharacter.generated.h"

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
	void OnHostGamePressed();
	void OnJoinGamePressed();
	void OnMeleeTutPressed();
	void OnRangeTutPressed();

	void OnShowFriends();
	void JoinFirstFriendInList();
	void InviteFirstPlayerInFriendslist();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* rightMesh;
	
	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* pointerMesh;

	UPROPERTY(EditAnywhere)
	float MenuSelectionRayCastRange;
};
