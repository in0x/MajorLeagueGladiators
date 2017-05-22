#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "HandMotionController.h"
#include "ChaperoneBounds.h"
#include "AbilitySystemInterface.h"
#include "MlgPlayerCharacter.generated.h"

class UHealthComponent;
class AMlgPlayerController;
class UWidgetComponent;
class UDamageReceiverComponent;
class UAbilitySystemComponent;
class UGameplayAbilitySet;
class UMlgAbilitySet;
class UVRControllerComponent;
class AMlgGrippableMeshActor;
class USteamVRChaperoneComponent;
class UTriggerZoneComponent;
class UGameplayAbility;
class UPredictedEffectsComponent;
class UPlayerDeathComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityMoveTargetLocationSet, FVector, NewLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityActivated, TSubclassOf<UGameplayAbility>, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityUseFail, TSubclassOf<UGameplayAbility>, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityUseSuccess, TSubclassOf<UGameplayAbility>, AbilityType, float, CooldownSeconds);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgPlayerCharacter : public AVRSimpleCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AMlgPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void SetLeftTriggerStatus(float Value);
	void SetRightTriggerStatus(float Value);
	
	void OnLeftTriggerClicked();
	void OnLeftTriggerReleased();
	void OnRightTriggerClicked();
	void OnRightTriggerReleased();
	void OnSideGripButtonLeft();
	void OnSideGripButtonRight();

	UFUNCTION(NetMulticast, reliable)
	void EnableActorCollison_NetMulticast(bool bNewActorEnableCollision);

	UFUNCTION(NetMulticast, reliable)
	void LaunchCharacter_NetMulticast(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

	UFUNCTION(NetMulticast, reliable)
	void StopMovementImmediately_NetMulticast();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	USkeletalMeshComponent* GetMotionControllerMesh(EControllerHand Hand);
	UVRControllerComponent* GetMotionController(EControllerHand Hand);
	AMlgGrippableMeshActor* GetAttachedWeapon();

	FVector CalcFeetPosition() const;

	UFUNCTION(BlueprintCallable)
	FVector GetProjectedLocation() const;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "AbilityMoveTargetLocation Set"))
	FAbilityMoveTargetLocationSet OnAbilityMoveTargetLocationSet;

	void SetAbilityMoveTargetLocation(FVector Location);

	void InvalidateAbilityMoveTargetLocation();

	virtual void Tick(float DelataTime) override;

	FAbilityActivated OnAbilityActivated;
	FAbilityUseFail OnAbilityUseFail;
	FAbilityUseSuccess OnAbilityUseSuccess;

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* rightMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* headMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* bodyMesh2;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UMaterialInterface* multiToolMaterial;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UMaterialInterface* bodyMaterial;

	UPROPERTY(EditAnywhere)
	UPlayerDeathComponent* deathComponent;

private:
	std::unique_ptr<HandMotionController> pHandMotionController;
	std::unique_ptr<ChaperoneBounds> pChaperoneBounds;

	FVector abilityMoveTargetLocation;

	UPROPERTY(EditAnywhere)
	USteamVRChaperoneComponent* chaperone;

	UPROPERTY(EditAnywhere)
	UHealthComponent* healthComp;

	UPROPERTY(EditAnywhere)
	UDamageReceiverComponent* dmgReceiverComp;

	UPROPERTY(EditAnywhere)
	FVector BodyOffsetFromHead;

	UPROPERTY(EditAnywhere)
	UTriggerZoneComponent* healthTriggerZone;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgGrippableMeshActor> startWeaponClass;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AttachedWeapon)
	AMlgGrippableMeshActor* attachedWeapon;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* hudHealth;

	UPROPERTY(EditAnywhere)
	UAbilitySystemComponent* abilitySystemComponent;

	UPROPERTY(EditAnywhere)
	UPredictedEffectsComponent* predictedEffectsComponent;

	UPROPERTY(EditAnywhere)
	TAssetSubclassOf<UMlgAbilitySet> abilitySetClass;

	UPROPERTY(Transient)
	const UMlgAbilitySet* cachedAbilitySet;

	const UMlgAbilitySet* GetOrLoadAbilitySet();

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

	void SpawnWeapon();

	UFUNCTION()
	void OnHealthChanged(float newHealthPercentage, float oldHealthPercentage);

	UFUNCTION()
	void OnRep_AttachedWeapon();

	void OnAttachedWeaponSet();

	virtual void BecomeViewTarget(APlayerController* PC) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const { return abilitySystemComponent; }

	UFUNCTION()
	void OnLand(const FHitResult& hit);
};
