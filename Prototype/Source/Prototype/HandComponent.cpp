// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HandComponent.h"


// Sets default values for this component's properties
UHandComponent::UHandComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
}

// Called when the game starts
void UHandComponent::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void UHandComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UHandComponent::SetControllerHand(EControllerHand newHand)
{
	hand = newHand;

	FVector offset(75, 25, 0);
	offset.Y *= hand == EControllerHand::Right ? -1.f : 1.f;
	AddLocalOffset(offset);
}

//bool UHandComponent::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
//{
//	return true;
//}
//
//ETrackingStatus UHandComponent::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
//{
//	return ETrackingStatus::Tracked;
//}


