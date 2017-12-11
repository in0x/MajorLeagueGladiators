// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MajorLeagueGladiator, "MajorLeagueGladiator" );
DEFINE_LOG_CATEGORY(DebugLog);

bool g_IsVREnabled()
{
	bool isEnabled = (GEngine && GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() != nullptr && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected());
	return isEnabled;
}