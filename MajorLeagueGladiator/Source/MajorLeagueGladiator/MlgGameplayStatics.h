// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MlgGameplayStatics.generated.h"

namespace ESplineMeshAxis { enum Type; }
class AMlgPlayerState;
class USplineMeshComponent;

USTRUCT()
struct FSplineMeshComponentParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "SplineMeshParams")
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "SplineMeshParams")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "SplineMeshParams")
	FVector2D StartScale;

	UPROPERTY(EditAnywhere, Category = "SplineMeshParams")
	FVector2D EndScale;

	UPROPERTY(EditAnywhere, Category = "SplineMeshParams")
	TEnumAsByte<ESplineMeshAxis::Type> SplineMeshAxis;

	FSplineMeshComponentParams();
};

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMlgGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static bool CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealerState, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver);
	static void ConfigureSplineMesh(USplineMeshComponent* splineMesh, const FSplineMeshComponentParams& params); 
	static void SetSplineMeshFromPath(USplineMeshComponent* splineMesh, const FTransform& parentTransform, const TArray<FPredictProjectilePathPointData>& path);
};
