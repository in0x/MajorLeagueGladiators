// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TargetingSplineMeshComponent.h"

UTargetingSplineMeshComponent::UTargetingSplineMeshComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> splineMaterial(TEXT("Material'/Game/Materials/M_SplineArcMat.M_SplineArcMat'"));
	material = splineMaterial.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> splineStaticMeshObject(TEXT("StaticMesh'/Game/MVRCFPS_Assets/BeamMesh.BeamMesh'"));
	SetStaticMesh(splineStaticMeshObject.Object);

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UTargetingSplineMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	SetMobility(EComponentMobility::Movable);

	SetMaterial(0, UMaterialInstanceDynamic::Create(material, nullptr));
	
	SetStartScale(FVector2D(20.f ,5.f));
	SetEndScale(FVector2D(20.f, 5.f));
	SetForwardAxis(ESplineMeshAxis::Type::X);
}

void UTargetingSplineMeshComponent::SetIsTargetValid(bool bIsTargetValid)
{
	UMaterialInstanceDynamic* matInstance = CastChecked<UMaterialInstanceDynamic>(GetMaterial(0));

	if (bIsTargetValid)
	{
		matInstance->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
	}
	else
	{
		matInstance->SetVectorParameterValue(FName("Color"), FLinearColor::Red);
	}
}

void UTargetingSplineMeshComponent::SetFromProjectilePath(const TArray<FPredictProjectilePathPointData>& path)
{
	float timeInFlight = path.Last().Time; // Need to multiply tangents with this to properly take length into account.
	updateStartAndEnd(path[0].Location, path.Last().Location, path[0].Velocity * timeInFlight, path.Last().Velocity * timeInFlight);
}

void UTargetingSplineMeshComponent::SetFromRayCast(FVector Start, FVector End, bool bDidHit)
{
	FVector tangent = End - Start;
	updateStartAndEnd(Start, End, tangent, tangent);
	SetIsTargetValid(bDidHit);
}

void UTargetingSplineMeshComponent::updateStartAndEnd(const FVector& start, const FVector& end, const FVector& startTangent, const FVector& endTangent)
{
	FTransform parentTransform = GetOwner()->GetTransform();

	SetStartAndEnd( parentTransform.InverseTransformPosition(start),
					parentTransform.InverseTransformVector(startTangent),
					parentTransform.InverseTransformPosition(end),
					parentTransform.InverseTransformVector(endTangent));

	UpdateMesh();
}

FVector UTargetingSplineMeshComponent::GetStartPositionWorld() const
{
	return GetOwner()->GetTransform().TransformFVector4(GetStartPosition());
}

FVector UTargetingSplineMeshComponent::GetEndPositionWorld() const
{
	return GetOwner()->GetTransform().TransformFVector4(GetEndPosition());
}
