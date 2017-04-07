// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MajorLeagueGladiator, "MajorLeagueGladiator" );
DEFINE_LOG_CATEGORY(DebugLog);

bool g_IsVREnabled()
{
	bool isEnabled = FParse::Param(FCommandLine::Get(), TEXT("vr")) || (GEngine && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHMDConnected());
	return isEnabled;
}