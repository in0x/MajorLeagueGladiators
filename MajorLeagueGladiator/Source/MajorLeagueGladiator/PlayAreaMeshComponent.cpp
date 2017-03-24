// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayAreaMeshComponent.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "IChaperoneBounds.h"

UPlayAreaMeshComponent::UPlayAreaMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("StaticMesh'/Game/VirtualReality/Meshes/1x1_cube.1x1_cube'"));
	SetStaticMesh(cubeMesh.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> cubeMaterial(TEXT("MaterialInstanceConstant'/Game/VirtualReality/Materials/MI_ChaperoneOutline.MI_ChaperoneOutline'"));
	material = cubeMaterial.Object;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPlayAreaMeshComponent::BeginPlay()
{
	SetMobility(EComponentMobility::Movable);
	SetMaterial(0, UMaterialInstanceDynamic::Create(material, nullptr));
}

void UPlayAreaMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!g_IsVREnabled())
	{
		SetWorldScale3D(FVector(100, 100, 100));
		return;
	}
	
	FVector outRectCenter; 
	FRotator outRotator;
	float outLengthX = 0.f;
	float outLengthY = 0.f;

	TArray<IChaperoneBounds*> chaperoneBounds = IModularFeatures::Get().GetModularFeatureImplementations<IChaperoneBounds>(IChaperoneBounds::GetModularFeatureName());
	TArray<FVector> bounds;

	if (chaperoneBounds.Num() > 0)
	{
		bounds = chaperoneBounds[0]->GetTrackingBounds();
		UKismetMathLibrary::MinimumAreaRectangle(GetWorld(),
			bounds,
			FVector(0, 0, 1),
			outRectCenter,
			outRotator,
			outLengthX,
			outLengthY);
	}
	else
	{
		UE_LOG(DebugLog, Error, TEXT("Could not find IChaperoneBounds feature, PlayAreaMesh cannot draw correct position"));
	}

	FQuat curRot;
	FVector curLoc;
	GEngine->HMDDevice->GetCurrentOrientationAndPosition(curRot, curLoc);
	curLoc.Z = 0;

	SetWorldScale3D(FVector(outLengthX, outLengthY, 100));
	SetRelativeRotation(outRotator);
	AddRelativeLocation(curLoc);
}

void UPlayAreaMeshComponent::SetIsTargetValid(bool bIsTargetValid)
{
	if (bIsTargetValid)
	{
		this->SetVisibility(true);
	}
	else
	{
		this->SetVisibility(false);
	}
}