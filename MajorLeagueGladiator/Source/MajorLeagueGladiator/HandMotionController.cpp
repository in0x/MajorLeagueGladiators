#include "MajorLeagueGladiator.h"
#include "HandMotionController.h"
#include "MlgPlayerCharacter.h"

HandMotionController::HandMotionController(AMlgPlayerCharacter* PlayerCharacter)
	: playerChar(PlayerCharacter)
{
	IModularFeatures::Get().RegisterModularFeature(IMotionController::GetModularFeatureName(), this);
}

HandMotionController::~HandMotionController()
{
	IModularFeatures::Get().UnregisterModularFeature(IMotionController::GetModularFeatureName(), this);
}

bool HandMotionController::GetControllerOrientationAndPosition(int32 ControllerIndex, EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
{
	if (DeviceHand != EControllerHand::Left && DeviceHand != EControllerHand::Right)
	{
		UE_LOG(LogTemp, Error, TEXT("HandMotionController was queried with neither left or right"));
		return false;
	}

	auto trafo = playerChar->GetActorTransform();

	FVector offset(75, 25, 0);
	offset.Y *= DeviceHand == EControllerHand::Left ? -1.f : 1.f;
	trafo.AddToTranslation(offset);

	OutPosition = offset;
	OutOrientation = FRotator{};

	return true;
}

ETrackingStatus HandMotionController::GetControllerTrackingStatus(int32 ControllerIndex, EControllerHand DeviceHand) const
{
	return ETrackingStatus::Tracked;
}

