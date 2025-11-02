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
	TObjectPtr<UImage> EyeImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> VeinsImage;

public:
	void SetBlurEffectThresholdReached(const bool bReached) { bBlurEffectThresholdReached = bReached; }
	bool GetBlurEffectThresholdReached() const { return bBlurEffectThresholdReached; }
};
