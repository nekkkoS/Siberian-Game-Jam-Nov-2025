// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayableCharacterPlayerController.generated.h"

class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API APlayableCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);


	// ----- Blinking -----

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BlinkAction;

	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	TSubclassOf<UUserWidget> BlinkOverlayClass;

	// Минимальная длительность моргания
	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	float MinBlinkDuration = 1.f;

private:
	void BlinkStart();
	void BlinkEnd();

	UPROPERTY()
	UUserWidget* BlinkOverlay;

	// Флаг активного моргания
	bool bIsBlinking = false;

	// Время, когда моргание началось
	float BlinkStartTime = 0.f;
};
