// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EyesightOverlayWidget.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UEyesightOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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

	virtual void NativeConstruct() override;
	void BlurTimerTick();

	UFUNCTION()
	void OnBlinkingEnded();

private:
	FTimerHandle BlurTimerHandle;
	TScriptInterface<class IBlinkingProviderInterface> BlinkingProviderInterface;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> BlinkPrompt;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBackgroundBlur> BackgroundBlur;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EyeImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> VeinsImage;

	bool bHasShownBlinkHint = false;

	// Чтобы запретить игроку моргнуть раньше времени
	bool bCanBlinkNow = false;

	UPROPERTY(EditDefaultsOnly, Category = "Eyesight | Blink")
	TSubclassOf<UUserWidget> BlinkHintWidgetClass;

	UPROPERTY()
	UUserWidget* BlinkHintWidget;
};
