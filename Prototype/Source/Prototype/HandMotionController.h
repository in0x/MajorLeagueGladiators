#pragma once
#include "IMotionController.h"

class APlayerCharacter;

/*
* NOTE(Phil):
* This serves as the IMotionController IModularfeature when no actual Motion
* controllers are connected. Since the VRExpansion::UGripMotionController
* gets the MotionController through IModularFeatures, this class takes the 
* SteamVRController's place in non-VR mode. We can therefore still use all 
* MotionController features without having a Vive connected. Instead of getting
* the MotionController's position from SteamVR, they are simply placed relative
* to the PlayerCharacter by this class. An instance of this class is owned by 
* APlayerCharacter.
*/
class PROTOTYPE_API HandMotionController :  public IMotionController
{
public:
	HandMotionController(APlayerCharacter* playerCharacter);
	virtual ~HandMotionController();
	
	virtual bool GetControllerOrientationAndPosition(int32 controllerIndex, EControllerHand deviceHand, FRotator& outOrientation, FVector& outPosition) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(int32 controllerIndex, EControllerHand deviceHand) const override;
	
private:
	APlayerCharacter* playerChar;
};
