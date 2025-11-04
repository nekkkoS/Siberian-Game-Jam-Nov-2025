// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../../Interfaces/BlinkingProviderInterface.h"
#include "PlayableCharacterPlayerController.generated.h"

class UPauseMenuWidget;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API APlayableCharacterPlayerController : public APlayerController, public IBlinkingProviderInterface
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

private:
	void PlayFootStepSound(APawn* ControlledPawn) const;


	// ----- Blinking -----

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BlinkAction;

	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	TSubclassOf<UUserWidget> BlinkOverlayClass;

	// Минимальная длительность моргания
	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	float MinBlinkDuration = 1.f;


	UPROPERTY(EditDefaultsOnly, Category = "Overlay | Blinking")
	TSubclassOf<class UEyesightOverlayWidget> EyesightOverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UEyesightOverlayWidget> EyesightOverlayWidget;

private:
	void BlinkStart();
	void BlinkEnd();

	UPROPERTY()
	UUserWidget* BlinkOverlay;

	// Флаг активного моргания
	bool bIsBlinking = false;

	// Время, когда моргание началось
	float BlinkStartTime = 0.f;

	FOnBlinkingEndedSignature OnBlinkingEndedDelegate;
	FOnEyesightOverlayReadySignature OnEyesightOverlayReadyDelegate;
	
	// ----- Pause -----

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	void OnPauseMenuToggle();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	float MouseSensitivity = 1.0f;

public:
	virtual UEyesightOverlayWidget* GetEyesightOverlayWidget_Implementation() override;
	virtual FOnBlinkingEndedSignature& ProvideOnBlinkingEndedDelegate() override;
	virtual FOnEyesightOverlayReadySignature& ProvideOnEyesightOverlayReadyDelegate() override;
	void SetMouseSensitivity(const float Value) { MouseSensitivity = Value; }
	float GetMouseSensitivity() const { return MouseSensitivity; }

	
	// ----- Saving -----
	
private:

	void OnSaveGame();

	void OnLoadLevel();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SaveGameAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LoadLevelAction;
};
