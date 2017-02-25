#pragma once 

#include "GameplayAbilityTargetActor.h"
#include <functional>
#include "GameplayAbilityTargetActor_Raycast.generated.h"

UENUM(BlueprintType)
namespace ERaycastTargetDirection
{
	enum Type
	{
		ForwardVector				UMETA(DisplayName = "ForwardVector"),
		UpVector					UMETA(DisplayName = "UpVector"),
		ComponentRotation			UMETA(DisplayName = "ComponentRotation"),
	};
}

using RaycastTargetEvaluationFunc = std::function<bool(const FHitResult&)>;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_Raycast : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_Raycast();

	virtual void Tick(float DeltaSeconds) override;
	virtual bool IsConfirmTargetingAllowed() override;
	
	TArray<AActor*> IgnoredActors;
	float MaxRange;
	ERaycastTargetDirection::Type aimDirection;
	RaycastTargetEvaluationFunc EvalTargetFunc;
	bool bShouldBroadcastResult;

private:
	FGameplayAbilityTargetDataHandle makeDataHandle(const FHitResult& Result) const;
};
