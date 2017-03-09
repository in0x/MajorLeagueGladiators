#pragma once

class MAJORLEAGUEGLADIATOR_API CollisionStatics
{
public:
	const static FName HITSCAN_TRACE_CHANNEL_NAME;
	const static FName GRIPSCAN_TRACE_CHANNEL_NAME;

	static TEnumAsByte<ECollisionChannel> GetCollsionChannelByName(FName ChannelName);
};
