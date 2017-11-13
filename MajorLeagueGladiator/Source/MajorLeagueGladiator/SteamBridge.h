// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SteamBridge.generated.h"

/*
NOTE(Phil): This is a singleton used to hide direct accesses to the SteamAPI. 
			Currently it is used to get ProfileAvatars, but it may also be
			used in the future if we need other Steam functionality not 
			implemented by IOnlineSubsystem.
*/

class SteamCallbackHandler;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAvatarDownloadedDelegate, int32, FriendIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamAvatarDownloadedDelegate, int32, FriendIndex);

UCLASS()
class MAJORLEAGUEGLADIATOR_API USteamBridge : public UObject
{
	GENERATED_BODY()
	
private:
	USteamBridge();
	
	SteamCallbackHandler* steamCallbacks;

	UFUNCTION()
	void OnAvatarDownloaded(int32 FriendIndex);

public:
	~USteamBridge();

	// Returns nullptr if SteamAPI fails to initialize.
	static USteamBridge* Get();

	UTexture2D* CreateAvatarTexture(int32 FriendIndex);

	// Returns true if Avatar is availible and LoadAvatarData() can
	// be called immediately. Returns bool if Avatar has to be downloaded.
	// In this case wait for OnAvatarLoaded to be broadcast with your FriendIndex
	// before calling LoadAvatarData();
	bool RequestAvatarData(int32 FriendIndex);

	void LoadAvatarData(int32 FriendIndex, UTexture2D* OutAvatar);

	void ResetSteamAchievementsAndStats();

	FOnAvatarDownloadedDelegate AvatarDownloaded;
};
