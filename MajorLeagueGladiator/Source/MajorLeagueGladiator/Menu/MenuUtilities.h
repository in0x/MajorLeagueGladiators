#pragma once

class UStaticMeshComponent;
class UWidgetComponent;
class UWidgetInteractionComponent;

namespace MenuUtilities
{
	void AttachMenu(UStaticMeshComponent* Left, UStaticMeshComponent* Right, UWidgetComponent* Widget,
		UWidgetInteractionComponent* WidgetInteraction, UStaticMeshComponent* MenuPointerMesh, bool InConstructor);

}
