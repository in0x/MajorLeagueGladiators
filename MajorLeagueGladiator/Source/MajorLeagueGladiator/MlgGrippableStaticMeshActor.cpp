// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "MlgPlayerController.h"

void AMlgGrippableStaticMeshActor::OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation)
{
	AMlgPlayerController* myController = CastChecked<AMlgPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	myController->ClientPlayForceFeedback(myController->rumbleTest, false,FName());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Gripped Lucas"));
}


