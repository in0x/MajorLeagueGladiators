#pragma once 

#include "GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_PredictProjectile.generated.h"

UENUM(BlueprintType)
namespace EPickMoveLocationTargeting
{
	enum Type
	{
		FromController				UMETA(DisplayName = "FromController"),
		FromPlayerCapsule			UMETA(DisplayName = "FromPlayerCapsule"),
	};
}

class UVRControllerComponent;
class USplineMeshComponent;
class AMlgPlayerCharacter;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_PredictProjectile : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_PredictProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool IsConfirmTargetingAllowed() override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	
	float TargetProjectileSpeed;
	float TargetProjectileFlightTime;
	
	TArray<AActor*> IgnoredActors;
	
	EPickMoveLocationTargeting::Type targetingType;

private:
	FGameplayAbilityTargetDataHandle makeDataHandle();
	
	void GetPlayerCapsuleFromAbility(AMlgPlayerCharacter* owner);
	void GetVrControllerFromAbility(AMlgPlayerCharacter* owner);
	
	bool PickTarget();

	UPROPERTY(EditAnywhere)
	USplineMeshComponent* splineMesh;
	UMaterialInterface* splineMeshMat;
	UStaticMesh* splineStaticMesh;

	UCapsuleComponent* playerCapsule;
	UVRControllerComponent* vrController;

	FPredictProjectilePathResult predictResult;
	FVector launchVelocity;
};
