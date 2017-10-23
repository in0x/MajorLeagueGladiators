// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SteamBridge.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv139/sdk/public/steam/isteamuser.h"

class SteamCallbackHandler
{
public:
	SteamCallbackHandler();

	FSteamAvatarDownloadedDelegate AvatarLoaded;

	void LoadSteamIDs();

private:
	STEAM_CALLBACK(SteamCallbackHandler, OnAvatarImageLoaded, AvatarImageLoaded_t);

	TArray<CSteamID> idToIndex;
};

SteamCallbackHandler::SteamCallbackHandler()
{
	LoadSteamIDs();
}

void SteamCallbackHandler::OnAvatarImageLoaded(AvatarImageLoaded_t* cbData)
{
	int32 friendIndex = 0;
	check(cbData);
	bool bFoundIndex = idToIndex.Find(cbData->m_steamID, friendIndex);
	check(bFoundIndex);

	AvatarLoaded.Broadcast(friendIndex);
}

void SteamCallbackHandler::LoadSteamIDs()
{
	idToIndex.Empty();

	int32 friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	idToIndex.Reserve(friendCount);

	for (int32 i = 0; i < friendCount; ++i)
	{
		CSteamID friendID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
		idToIndex.Add(friendID);
	}
}

USteamBridge::USteamBridge()
{
	AddToRoot();

	if (!GEngine->IsEditor())
	{
		steamCallbacks = new SteamCallbackHandler;
		steamCallbacks->AvatarLoaded.AddDynamic(this, &USteamBridge::OnAvatarDownloaded);
	}
}

USteamBridge::~USteamBridge() 
{
	delete steamCallbacks;
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

UTexture2D* USteamBridge::CreateAvatarTexture(int32 FriendIndex)
{
	check(FriendIndex >= 0 && FriendIndex < SteamFriends()->GetFriendCount(k_EFriendFlagImmediate));

	uint32 width = 0;
	uint32 height = 0;

	SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(FriendIndex, k_EFriendFlagImmediate);
	int friendPictureHandle = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	bool bDimensionsKnown = SteamUtils()->GetImageSize(friendPictureHandle, &width, &height);

	if (!bDimensionsKnown || width <= 0 || height <= 0)
	{
		return nullptr;
	}

	UTexture2D* avatar = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	return avatar;
}

void USteamBridge::OnAvatarDownloaded(int32 FriendIndex)
{
	AvatarDownloaded.Broadcast(FriendIndex);
}

bool USteamBridge::RequestAvatarData(int32 FriendIndex)
{
	check(FriendIndex >= 0 && FriendIndex < SteamFriends()->GetFriendCount(k_EFriendFlagImmediate));

	CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(FriendIndex, k_EFriendFlagImmediate);
	int friendPictureHandle = SteamFriends()->GetLargeFriendAvatar(friendSteamID);

	if (friendPictureHandle <= 0)
	{
		return false;
	}

	uint32 width = 0;
	uint32 height = 0;

	bool bDimensionsKnown = SteamUtils()->GetImageSize(friendPictureHandle, &width, &height);

	if (!bDimensionsKnown || width <= 0 || height <= 0)
	{
		return false;
	}

	return true;
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



