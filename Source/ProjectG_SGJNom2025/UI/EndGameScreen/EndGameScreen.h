// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameScreen.generated.h"

class UButton;
class UImage;
/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UEndGameScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void PlayEndGameSequence();

protected:

	UPROPERTY(meta=(BindWidget))
	UImage* BackGroundImg;
	
	UPROPERTY(meta=(BindWidget))
	UImage* Image1;

	UPROPERTY(meta=(BindWidget))
	UImage* Image2;

	UPROPERTY(meta=(BindWidget))
	UImage* Image3;

	UPROPERTY(meta=(BindWidget))
	UButton* ExitBtn;

	/** Таймер для плавного появления */
	FTimerHandle AnimationTimerHandle;

	/** Текущее состояние анимации */
	int32 StepIndex = 0;

	/** Время плавного появления каждого элемента */
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float FadeDuration = 1.0f;

	/** Время задержки между элементами */
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float DelayBetweenElements = 0.5f;

private:
	/** Функция для пошагового появления элементов */
	void AnimateNextStep();
    
	/** Вспомогательная функция для плавного изменения прозрачности */
	void FadeInWidget(UWidget* Widget);
};
