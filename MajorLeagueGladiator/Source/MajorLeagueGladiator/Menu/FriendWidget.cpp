// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "FriendWidget.h"

void UFriendWidget::ChangeFriendIndex(int32 Index)
{
	friendIndex = Index;
}

void UFriendWidget::EmitJoinFriendRequested() const
{
	JoinFriendRequested.Broadcast(friendIndex);
}