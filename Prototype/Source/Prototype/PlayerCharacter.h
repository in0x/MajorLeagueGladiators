#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "HandMotionController.h"
#include "TeleportComponent.h"

#include "PlayerCharacter.generated.h"

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
	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	void MoveForward(float value);
	void MoveRight(float value);
	
	void OnLeftTriggerAxis(float value);
	void OnRightTriggerAxis(float value);
	void OnLeftTriggerClicked();
	void OnLeftTriggerReleased();
	void OnRightTriggerClicked();
	void OnRightTriggerReleased();
	void OnTeleportPressedLeft();
	void OnTeleportPressedRight();
	void OnTeleportReleased();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* rightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTeleportComponent* teleportComp;

private:
	std::unique_ptr<HandMotionController> pHandMotionController;

	USphereComponent* leftGrabSphere;
	USphereComponent* rightGrabSphere;

#pragma region serverFunctions
	UFUNCTION(Server, WithValidation, reliable)
	void Server_LeftHandGrab();

	UFUNCTION(Server, WithValidation, reliable)
	void Server_LeftHandDrop();

	UFUNCTION(Server, WithValidation, reliable)
	void Server_RightHandGrab();

	UFUNCTION(Server, WithValidation, reliable)
	void Server_RightHandDrop();
#pragma endregion

};
