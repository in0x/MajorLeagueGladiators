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

UCLASS()
class MAJORLEAGUEGLADIATOR_API USteamBridge : public UObject
{
	GENERATED_BODY()
	
private:
	USteamBridge();
	
public:
	// Returns nullptr if SteamAPI fails to initialize.
	static USteamBridge* Get();

	UTexture2D* CreateAvatarTexture();

	void LoadAvatarData(int32 FriendIndex, UTexture2D* OutAvatar);
};
