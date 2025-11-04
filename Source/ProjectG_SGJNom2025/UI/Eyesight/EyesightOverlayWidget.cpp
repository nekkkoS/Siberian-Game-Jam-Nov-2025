// Copyright Offmeta

#include "EyesightOverlayWidget.h"

#include "Components/BackgroundBlur.h"
#include "Kismet/GameplayStatics.h"
#include "../../Interfaces/BlinkingProviderInterface.h"
#include "Components/Image.h"
#include "ProjectG_SGJNom2025/Character/Player/PlayableCharacter.h"

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
		return;
	
	float NewBlur = BackgroundBlur->GetBlurStrength() + BlurIncreaseWithEachTimerTick;
	BackgroundBlur->SetBlurStrength(NewBlur);

	// --- Добавляем постепенное затемнение и изображение вен ---
	float Progress = FMath::Clamp(NewBlur / BlurScreenTillThisStrength, 0.f, 1.f);
	EyesightClarity = Progress;
	float TargetOpacity = Progress * MaxDarkenOpacity;

	if (DarkenEdgesImage)
		DarkenEdgesImage->SetOpacity(TargetOpacity);

	if (VeinsImage)
		VeinsImage->SetOpacity(FMath::Max(0.f, TargetOpacity - 0.15f));

	// Показать подсказку моргания
	if (!bHasShownBlinkHint && NewBlur >= TimeForShowHint)
	{
		ShowBlinkHint();
		bHasShownBlinkHint = true;
		bCanBlinkNow = true;
	}

	// Смерть, если достигли максимума
	if (NewBlur >= BlurScreenTillThisStrength)
	{
		if (APlayerController* PCtrl = GetOwningPlayer())
		{
			if (APawn* Pawn = PCtrl->GetPawn())
			{
				if (APlayableCharacter* PlayableChar = Cast<APlayableCharacter>(Pawn))
				{
					PlayableChar->Die();
				}
			}
		}
		ResetBlurTimer();
	}

	UE_LOG(LogTemp, Warning, TEXT("Blur Strength: %f"), NewBlur);

	// Проверка порога критичности
	if (!bBlurEffectThresholdReached && NewBlur >= BlurThresholdCriticalValue * BlurScreenTillThisStrength)
	{
		bBlurEffectThresholdReached = true;
		OnBlurEffectCriticalThresholdReachedDelegate.Broadcast(bBlurEffectThresholdReached);
	}
}

void UEyesightOverlayWidget::OnBlinkingEnded()
{
	ResetBlurEffect();
	ResetBlurTimer();
	EyesightClarity = 0.0f;
	bBlurEffectThresholdReached = false;
	//OnBlurEffectCriticalThresholdReachedDelegate.Broadcast(bBlurEffectThresholdReached);
	StartBlurEffectTimer();
}

void UEyesightOverlayWidget::StopBlurEffect()
{
	if (BlurTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlurTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Blur effect stopped."));
	}
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

	if (DarkenEdgesImage)
	{
		DarkenEdgesImage->SetOpacity(0.0f);
	}

	if (VeinsImage)
	{
		VeinsImage->SetOpacity(0.0f);
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

void UEyesightOverlayWidget::ShowBlinkHint()
{
	if (!BlinkHintWidgetClass || BlinkHintWidget)
		return;

	BlinkHintWidget = CreateWidget<UUserWidget>(GetWorld(), BlinkHintWidgetClass);
	if (BlinkHintWidget && !BlinkHintWidget->IsInViewport())
	{
		BlinkHintWidget->AddToViewport();
	}
}

void UEyesightOverlayWidget::HideBlinkHint()
{
	if (BlinkHintWidget && BlinkHintWidget->IsInViewport())
	{
		BlinkHintWidget->RemoveFromParent();
		BlinkHintWidget = nullptr;
	}
}
