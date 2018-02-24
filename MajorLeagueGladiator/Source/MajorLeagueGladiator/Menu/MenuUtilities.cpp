#include "MajorLeagueGladiator.h"

#include "MenuUtilities.h"


#include "Components/StaticMeshComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
	const FName ATTACHMENT_SOCKET_NAME("Attachment");

	const FAttachmentTransformRules KeepRelativeRule(EAttachmentRule::KeepRelative, false);
	const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepRelative, true);


	void AttachInConstructor(USceneComponent* Child, USceneComponent* Parent, FName Socket)
	{
		Child->SetupAttachment(Parent, Socket);
	}
	void Attach(USceneComponent* Child, USceneComponent* Parent, FName Socket)
	{
		Child->DetachFromComponent(DetachmentRules);
		Child->AttachToComponent(Parent, KeepRelativeRule, Socket);
	}
}

void MenuUtilities::AttachMenu(UStaticMeshComponent * Left, UStaticMeshComponent * Right, UWidgetComponent * Widget,
	UWidgetInteractionComponent * WidgetInteraction, UStaticMeshComponent * MenuPointerMesh, bool InConstructor)
{
	auto AttachFunction = InConstructor ? &AttachInConstructor : Attach;

	AttachFunction(Widget, Left, FName(TEXT("Touch")));

	AttachFunction(WidgetInteraction, Right, AIM_SOCKET_NAME);

	AttachFunction(MenuPointerMesh, Right, AIM_SOCKET_NAME);

	MenuPointerMesh->SetRelativeRotation(FQuat::Identity);
	MenuPointerMesh->SetRelativeLocation(FVector::ZeroVector);	
}
