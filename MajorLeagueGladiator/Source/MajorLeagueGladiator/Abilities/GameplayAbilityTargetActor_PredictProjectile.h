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

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_PredictProjectile : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_PredictProjectile();

	virtual void Tick(float DeltaSeconds) override;
	virtual bool IsConfirmTargetingAllowed() override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	
	float TargetProjectileSpeed;
	
	TArray<AActor*> IgnoredActors;
	
	EPickMoveLocationTargeting::Type targetingType;

	bool bShouldBroadcastResult;

private:
	FGameplayAbilityTargetDataHandle makeDataHandle(FPredictProjectilePathResult& PredictResult, const FVector& LaunchVelocity) const;
	
	void GetPlayerCapsuleFromAbility(UGameplayAbility* Ability);
	void GetVrControllerFromAbility(UGameplayAbility* Ability);

	bool PickTargetFromPlayerCapsule(FPredictProjectilePathResult& OutResult, FVector& OutLaunchVelocity);
	bool PickTargetFromVrController(FPredictProjectilePathResult& OutResult, FVector& OutLaunchVelocity);
	
	UCapsuleComponent* playerCapsule;
	UVRControllerComponent* vrController;
};
