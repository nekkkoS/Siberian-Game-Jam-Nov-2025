// Copyright Offmeta

#include "EyesightOverlayWidget.h"

#include "Components/BackgroundBlur.h"
#include "Kismet/GameplayStatics.h"
#include "../../Interfaces/BlinkingProviderInterface.h"

void UEyesightOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetBlinkPromptVisibility(ESlateVisibility::Hidden);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0); PlayerController &&
		PlayerController->GetClass()->ImplementsInterface(UBlinkingProviderInterface::StaticClass()))
	{
		BlinkingProviderInterface = TScriptInterface<IBlinkingProviderInterface>(PlayerController);
	}
	else
	{
		BlinkingProviderInterface = nullptr;
	}

	if (BlinkingProviderInterface)
	{
		BlinkingProviderInterface->ProvideOnBlinkingEndedDelegate().AddUObject(this, &UEyesightOverlayWidget::OnBlinkingEnded);
	}

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

	// Check if the blur strength has reached the critical threshold and broadcast the event.
	if (!bBlurEffectThresholdReached && BackgroundBlur->GetBlurStrength() >= BlurThresholdCriticalValue * BlurScreenTillThisStrength)
	{
		bBlurEffectThresholdReached = true;
		OnBlurEffectCriticalThresholdReachedDelegate.Broadcast(bBlurEffectThresholdReached);
	}
}

void UEyesightOverlayWidget::OnBlinkingEnded()
{
	ResetBlurEffect();
	ResetBlurTimer();
	bBlurEffectThresholdReached = false;
	//OnBlurEffectCriticalThresholdReachedDelegate.Broadcast(bBlurEffectThresholdReached);
	StartBlurEffectTimer();
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
