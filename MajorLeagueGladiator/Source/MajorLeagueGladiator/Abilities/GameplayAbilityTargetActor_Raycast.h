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
class UTargetingSplineMeshComponent;
class UPlayAreaMeshComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_Raycast : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AGameplayAbilityTargetActor_Raycast(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual bool IsConfirmTargetingAllowed() override;
	virtual void ConfirmTargetingAndContinue() override;

	TArray<AActor*> IgnoredActors;
	
	float MaxRange;
	bool CanConfirmInvalidTarget;
	
	ERaycastTargetDirection::Type aimDirection;
	
	RaycastTargetEvaluationFunc EvalTargetFunc;
	
private:
	FGameplayAbilityTargetDataHandle makeDataHandle();

	FHitResult hitResult;

	UPROPERTY(EditAnywhere)
	UTargetingSplineMeshComponent* splineMesh;

	UPROPERTY(EditAnywhere)
	UPlayAreaMeshComponent* playAreaMesh;
};
