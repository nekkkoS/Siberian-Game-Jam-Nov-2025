// Copyright Offmeta


#include "DefaultHUD.h"

#include "Blueprint/UserWidget.h"
#include "../../UI/Eyesight/EyesightOverlayWidget.h"

void ADefaultHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerRef = GetOwningPlayerController();

	/*if (PlayerControllerRef.IsValid() && EyesightOverlayWidgetClass)
	{
		EyesightOverlayWidget = CreateWidget<UEyesightOverlayWidget>(PlayerControllerRef.Get(), EyesightOverlayWidgetClass);
		EyesightOverlayWidget->AddToViewport();
	}*/
}
