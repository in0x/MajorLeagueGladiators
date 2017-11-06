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

void UFriendWidget::EmitInviteFriendRequested() const
{
	InviteFriendRequested.Broadcast(friendIndex);
}

const FString& UFriendWidget::GetUsername() const
{
	return username;
}

int32 UFriendWidget::GetFriendIndex() const
{
	return friendIndex;
}

bool UFriendWidget::GetCanJoin() const
{
	return bCanJoin;
}

bool UFriendWidget::GetCanInvite() const
{
	return bCanInvite;
}