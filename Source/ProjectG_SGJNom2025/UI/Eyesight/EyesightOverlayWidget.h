// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EyesightOverlayWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlurEffectCriticalThresholdReachedSignature, bool /* bReached */);

class UImage;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UEyesightOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnBlurEffectCriticalThresholdReachedSignature OnBlurEffectCriticalThresholdReachedDelegate;

	UFUNCTION(BlueprintCallable)
	void SetBlinkPromptVisibility(ESlateVisibility _Visibility);

	UFUNCTION(BlueprintCallable)
	void StartBlurEffectTimer();

	UFUNCTION(BlueprintCallable)
	void ResetBlurEffect();

	UFUNCTION(BlueprintCallable)
	void ResetBlurTimer();

	UFUNCTION(BlueprintCallable)
	void ShowBlinkHint();

	UFUNCTION(BlueprintCallable)
	void HideBlinkHint();

	bool GetCanBlinkNow() const { return bCanBlinkNow; }

	UPROPERTY(BlueprintReadOnly, Category = "Eyesight | Blink")
	float EyesightClarity = 0.0f;

	/** Возвращает степень закрытости глаз (0 — чистое зрение, 1 — смерть) */
	UFUNCTION(BlueprintPure, Category = "Eyesight | Blink")
	float GetEyesightClarity() const { return EyesightClarity; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink")
	float BlurTimerTickRate = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink")
	bool BlurTimerShouldLoop = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink")
	float BlurTimerFirstDelay = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink")
	float BlurIncreaseWithEachTimerTick = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink")
	float BlurScreenTillThisStrength = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Eyesight | Blink",
		meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float BlurThresholdCriticalValue = 0.5f;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DarkenEdgesImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Eyesight | Blink")
	float DarkenIncreaseRate = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Eyesight | Blink")
	float MaxDarkenOpacity = 0.8f;

	virtual void NativeConstruct() override;
	void BlurTimerTick();

	UFUNCTION()
	void OnBlinkingEnded();

private:
	FTimerHandle BlurTimerHandle;
	TScriptInterface<class IBlinkingProviderInterface> BlinkingProviderInterface;

	bool bBlurEffectThresholdReached = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> BlinkPrompt;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBackgroundBlur> BackgroundBlur;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> VeinsImage;

	bool bHasShownBlinkHint = false;

	// Чтобы запретить игроку моргнуть раньше времени
	bool bCanBlinkNow = false;

	UPROPERTY(EditDefaultsOnly, Category = "Eyesight | Blink")
	float TimeForShowHint = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Eyesight | Blink")
	TSubclassOf<UUserWidget> BlinkHintWidgetClass;

	UPROPERTY()
	UUserWidget* BlinkHintWidget;

public:
	void SetBlurEffectThresholdReached(const bool bReached) { bBlurEffectThresholdReached = bReached; }
	bool GetBlurEffectThresholdReached() const { return bBlurEffectThresholdReached; }

	UFUNCTION(BlueprintCallable, Category="Eyesight | Blink")
	void StopBlurEffect();
	
};
