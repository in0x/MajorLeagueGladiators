#include "MajorLeagueGladiator.h"

#include "CollisionStatics.h"

const FName CollisionStatics::HITSCAN_TRACE_CHANNEL_NAME("HitScan");
const FName CollisionStatics::GRIPSCAN_TRACE_CHANNEL_NAME("GripScan");

TEnumAsByte<ECollisionChannel> CollisionStatics::GetCollsionChannelByName(FName ChannelName)
{
	const UCollisionProfile* collisionProfile = UCollisionProfile::Get();
	for (int32 channel = ECC_GameTraceChannel1; channel <= ECC_GameTraceChannel18; ++channel)
	{
		if (ChannelName == collisionProfile->ReturnChannelNameFromContainerIndex(channel))
		{
			return TEnumAsByte<ECollisionChannel>(channel);
		}
	}

	FString channelString = ChannelName.ToString();

	UE_LOG(LogTemp, Warning, TEXT("Collisionchannel \"%s\" not found!"), *channelString);
	return TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_MAX);
}
