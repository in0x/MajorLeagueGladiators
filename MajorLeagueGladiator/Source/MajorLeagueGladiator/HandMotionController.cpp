#include "MajorLeagueGladiator.h"
#include "HandMotionController.h"

HandMotionController::HandMotionController(ACharacter* PlayerCharacter)
	: playerChar(PlayerCharacter)
	, rotation(FRotator())
{
	IModularFeatures::Get().RegisterModularFeature(IMotionController::GetModularFeatureName(), this);
}

HandMotionController::~HandMotionController()
{
	IModularFeatures::Get().UnregisterModularFeature(IMotionController::GetModularFeatureName(), this);
}

FName HandMotionController::GetMotionControllerDeviceTypeName() const
{
	return FName("Non-VR Mock Controller");
}

void HandMotionController::SetCustomRotation(const FRotator& Rotation)
{
	rotation = Rotation;
}

bool HandMotionController::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	if (DeviceHand != EControllerHand::Left && DeviceHand != EControllerHand::Right)
	{
		UE_LOG(DebugLog, Error, TEXT("HandMotionController was queried with neither left or right"));
		return false;
	}

	auto trafo = playerChar->GetActorTransform();

	FVector offset(75, 25, 0);
	offset.Y *= DeviceHand == EControllerHand::Left ? -1.f : 1.f;
	trafo.AddToTranslation(offset);

	OutPosition = offset;
	OutOrientation = rotation;

	return true;
}

ETrackingStatus HandMotionController::GetControllerTrackingStatus(int32 ControllerIndex, EControllerHand DeviceHand) const
{
	return ETrackingStatus::Tracked;
}
