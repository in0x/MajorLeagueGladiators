// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "FriendlistWidget.h"

void UFriendlistWidget::EmitFriendsRefreshRequest()
{
	FriendsRefreshRequest.Broadcast();
}


