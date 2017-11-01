// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TravelMapCharacter.h"
#include "Menu/MenuGameMode.h"

ATravelMapCharacter::ATravelMapCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BaseEyeHeight = 0.0f;
	CrouchedEyeHeight = 0.0f;

	loadingWidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("MenuWidgetComponent"));
	ConstructorHelpers::FClassFinder<UUserWidget> loadingWidget(TEXT("/Game/BluePrints/LoadingWidgetBP"));
	loadingWidgetComponent->SetWidgetClass(loadingWidget.Class);
	loadingWidgetComponent->SetupAttachment(VRReplicatedCamera);
}

void ATravelMapCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!g_IsVREnabled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));
	}
}


