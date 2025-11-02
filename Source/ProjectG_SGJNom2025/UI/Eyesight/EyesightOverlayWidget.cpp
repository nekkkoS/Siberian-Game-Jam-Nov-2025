// Copyright Offmeta

#include "EyesightOverlayWidget.h"

#include "Components/BackgroundBlur.h"

void UEyesightOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetBlinkPromptVisibility(ESlateVisibility::Hidden);

	StartBlurEffectTimer();
}

void UEyesightOverlayWidget::BlurTimerTick()
{
	if (!BackgroundBlur)
	{
		return;
	}

	if (BackgroundBlur->GetBlurStrength() >= BlurScreenTillThisStrength)
	{
		ResetBlurTimer();
	}

	BackgroundBlur->SetBlurStrength(BackgroundBlur->GetBlurStrength() + BlurIncreaseWithEachTimerTick);
	UE_LOG(LogTemp, Warning, TEXT("Blur Strength: %f"), BackgroundBlur->GetBlurStrength());
}

void UEyesightOverlayWidget::SetBlinkPromptVisibility(ESlateVisibility _Visibility)
{
	if (BlinkPrompt)
	{
		BlinkPrompt->SetVisibility(_Visibility);
	}
}

void UEyesightOverlayWidget::StartBlurEffectTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		BlurTimerHandle,
		this,
		&UEyesightOverlayWidget::BlurTimerTick,
		BlurTimerTickRate,
		BlurTimerShouldLoop,
		BlurTimerFirstDelay);
}

void UEyesightOverlayWidget::ResetBlurEffect()
{
	if (BackgroundBlur)
	{
		BackgroundBlur->SetBlurStrength(0.0f);
	}
}

void UEyesightOverlayWidget::ResetBlurTimer()
{
	if (BlurTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlurTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Blur timer is reset"));
	}
}
