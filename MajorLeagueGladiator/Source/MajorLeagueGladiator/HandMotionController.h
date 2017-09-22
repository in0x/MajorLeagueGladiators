#pragma once
#include "IMotionController.h"

/*
* NOTE(Phil)
* This serves as the IMotionController IModularfeature when no actual Motion
* controllers are connected. Since the VRExpansion::UGripMotionController
* gets the MotionController through IModularFeatures, this class takes the 
* SteamVRController's place in non-VR mode. We can therefore still use all 
* MotionController features without having a Vive connected. Instead of getting
* the MotionController's position from SteamVR, they are simply placed relative
* to the PlayerCharacter by this class. An instance of this class is owned by 
* AMlgPlayerCharacter.
*/
class MAJORLEAGUEGLADIATOR_API HandMotionController :  public IMotionController
{
public:
	HandMotionController(ACharacter* PlayerCharacter);
	
	virtual ~HandMotionController();
	
	virtual FName GetMotionControllerDeviceTypeName() const override;
	
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const override;

	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;
	
private:
	ACharacter* playerChar;
};