#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "HandMotionController.h"
#include "MessageEndpoint.h"
#include "PlayerCharacter.generated.h"

class UTeleportComponent;
class UHealthComponent;
class APrototypePlayerController;
class UWidgetComponent;
class UDamageReceiverComponent;

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
	void OnStartGame();

private:
	std::unique_ptr<HandMotionController> pHandMotionController;

	FMessageEndpointPtr msgEndpoint;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* rightMesh;

	UPROPERTY(EditAnywhere)
	UTeleportComponent* teleportComp;

	UPROPERTY(EditAnywhere)
	UHealthComponent* healthComp;

	UPROPERTY(EditAnywhere)
	UDamageReceiverComponent* dmgReceiverComp;

	UPROPERTY(EditAnywhere)
	USphereComponent* leftGrabSphere;

	UPROPERTY(EditAnywhere)
	USphereComponent* rightGrabSphere;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* bodyMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> healthTriggerClass;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* hudWidgetHealth;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* hudTeleportCD;

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

	virtual void BecomeViewTarget(APlayerController* PC) override;
};
