// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SteamBridge.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/isteamuser.h"

USteamBridge::USteamBridge()
{
	AddToRoot();
}

USteamBridge* USteamBridge::Get()
{
	static USteamBridge* bridge = nullptr;

	if (nullptr == bridge)
	{
		if (!SteamAPI_Init())
		{
			return nullptr;
		}

		bridge = NewObject<USteamBridge>();
	}

	return bridge;
}

UTexture2D* USteamBridge::CreateAvatarTexture()
{
	uint32 width = 0;
	uint32 height = 0;

	CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(0, k_EFriendFlagImmediate);
	int friendPicture = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	SteamUtils()->GetImageSize(friendPicture, &width, &height);

	if (width <= 0 || height <= 0)
	{
		return nullptr;
	}

	UTexture2D* avatar = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	return avatar;
}

void USteamBridge::GetAvatar(int32 FriendIndex, UTexture2D* Avatar)
{
	check(FriendIndex < SteamFriends()->GetFriendCount(k_EFriendFlagImmediate));

	uint32 width = Avatar->GetSurfaceWidth();
	uint32 height = Avatar->GetSurfaceHeight();

	CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(FriendIndex, k_EFriendFlagImmediate);
	int friendPicture = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	uint8* mipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

	SteamUtils()->GetImageRGBA(friendPicture, mipData, 4 * height * width * sizeof(uint8));

	Avatar->PlatformData->Mips[0].BulkData.Unlock();

	Avatar->PlatformData->NumSlices = 1;
	Avatar->NeverStream = true;

	const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
	Avatar->Rename(*FString::Printf(TEXT("ProfilePicture%s"), ANSI_TO_TCHAR(friendName)));
	Avatar->UpdateResource();
}



