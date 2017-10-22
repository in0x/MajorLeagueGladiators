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
	int friendPictureHandle = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	bool success = SteamUtils()->GetImageSize(friendPictureHandle, &width, &height);

	if (!success || width <= 0 || height <= 0)
	{
		return nullptr;
	}

	UTexture2D* avatar = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	return avatar;
}

void USteamBridge::LoadAvatarData(int32 FriendIndex, UTexture2D* OutAvatar)
{
	check(FriendIndex >= 0 && FriendIndex < SteamFriends()->GetFriendCount(k_EFriendFlagImmediate));

	uint32 width = OutAvatar->GetSurfaceWidth();
	uint32 height = OutAvatar->GetSurfaceHeight();

	CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(FriendIndex, k_EFriendFlagImmediate);
	int friendPictureHandle = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	uint8* mipData = (uint8*)OutAvatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

	SteamUtils()->GetImageRGBA(friendPictureHandle, mipData, 4 * height * width * sizeof(uint8));

	OutAvatar->PlatformData->Mips[0].BulkData.Unlock();

	OutAvatar->PlatformData->NumSlices = 1;
	OutAvatar->NeverStream = true;

	const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
	OutAvatar->Rename(*FString::Printf(TEXT("ProfilePicture%s"), ANSI_TO_TCHAR(friendName)));
	OutAvatar->UpdateResource();
}



