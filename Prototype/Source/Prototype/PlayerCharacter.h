#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "HandMotionController.h"
#include "PlayerCharacter.generated.h"

class UTeleportComponent;

/**
 *
 */
UCLASS()
class PROTOTYPE_API APlayerCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnLeftTriggerClicked();
	void OnLeftTriggerReleased();
	void OnRightTriggerClicked();
	void OnRightTriggerReleased();
	void OnTeleportPressedLeft();
	void OnTeleportPressedRight();
	void OnTeleportReleased();
	void OnSideGripButtonLeft();
	void OnSideGripButtonRight();

private:
	std::unique_ptr<HandMotionController> pHandMotionController;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* rightMesh;

	UPROPERTY(EditAnywhere)
	UTeleportComponent* teleportComp;

	UPROPERTY(EditAnywhere)
	USphereComponent* leftGrabSphere;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* rightGrabSphere;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> healthTriggerClass;

	UFUNCTION(Server, WithValidation, reliable)
	void leftHandGrab_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void leftHandRelease_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void leftHandDrop_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void rightHandGrab_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void rightHandRelease_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void rightHandDrop_Server();

	UFUNCTION(Server, WithValidation, reliable)
	void requestTeleport_Server(FVector Location, FRotator Rotation);
	
	UFUNCTION(NetMulticast, reliable)
	void performTeleport_NetMulticast(FVector Location, FRotator Rotation);
};
