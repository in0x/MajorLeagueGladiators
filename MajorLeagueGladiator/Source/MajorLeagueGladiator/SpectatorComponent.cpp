// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SpectatorComponent.h"

USpectatorComponent::USpectatorComponent(const FObjectInitializer& ObjectInitializer /* = FObjectInitializer::Get() */)
	: Super(ObjectInitializer)
	, window(nullptr)
	, windowSize(1920, 1080)
{
	const static ConstructorHelpers::FObjectFinder<UMaterialInterface> matFinder(TEXT("Material'/Game/Materials/M_DebugView.M_DebugView'"));
	widgetMaterial = matFinder.Object;
}

void USpectatorComponent::SetSceneCapture(USceneCaptureComponent2D* SceneCapture)
{
	sceneCapture = SceneCapture;
}

void USpectatorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USpectatorComponent::Create()
{
	sceneCapture->DetailMode = EDetailMode::DM_Medium;
	sceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	if (!window.IsValid())
	{
		auto SlateWinRef = SNew(SWindow)
			.MinWidth(1920)
			.MinHeight(1080)
			//.Type(EWindowType::GameWindow)
			.UserResizeBorder(FMargin(0, 0))
			.UseOSWindowBorder(true)
			.AutoCenter(EAutoCenter::None)
			.Title(FText::FromString(TEXT("DebugSpectator")))
			.IsInitiallyMaximized(true/*false*/)
			.ScreenPosition(FVector2D(0, 0))
			.ClientSize(windowSize)
			//.CreateTitleBar(true)
			.SizingRule(ESizingRule::UserSized)
			/*.SupportsMaximize(true)
			.SupportsMinimize(true)*/
			/*.HasCloseButton(true)*/;

		window = SlateWinRef;

		img = NewObject<UImage>(GetWorld()->GetGameInstance());

		matInstance = UMaterialInstanceDynamic::Create(widgetMaterial, nullptr);
		img->SetBrushFromMaterial(matInstance);

		if (!sceneCapture->TextureTarget)
		{
			sceneCapture->TextureTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), windowSize.X, windowSize.Y);
			matInstance->SetTextureParameterValue(FName("RenderTex"), sceneCapture->TextureTarget);
		}

		window->SetContent(img->TakeWidget());

		FSlateApplication & SlateApp = FSlateApplication::Get();
		SlateApp.AddWindow(SlateWinRef, true);
	}

	GetOwner()->OnEndPlay.AddDynamic(this, &USpectatorComponent::EndPlayCallback);
}

void USpectatorComponent::EndPlayCallback(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	window->RequestDestroyWindow();
}