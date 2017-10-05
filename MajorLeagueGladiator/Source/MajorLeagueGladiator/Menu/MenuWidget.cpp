// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuWidget.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UPanelWidget* rootWidget = Cast<UPanelWidget>(GetRootWidget());

	grid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), TEXT("MainGrid")); 
	rootWidget->AddChild(grid);
	
	/*for (int32 slots = 0; slots < 3; ++slots)
	{
		rootWidget->Slots.Add(FSlot(FString::Printf("Slot_%d"), slots));
	}*/

	/*for (int32 slots = 0; slots < 3; ++slots)
	{
		grid->RowFill.Emplace(0.3);
		UWidget* button = WidgetTree->ConstructWidget<UButton>();
		grid->AddChildToGrid(button);
	}*/
}



