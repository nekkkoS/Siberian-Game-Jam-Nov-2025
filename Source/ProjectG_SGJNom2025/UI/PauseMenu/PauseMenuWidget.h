// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class USlider;
class UButton;
/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeBtn;

	UPROPERTY(meta = (BindWidget))
	USlider* VolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* SensitivitySlider;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnVolumeChanged(float Value);

	UFUNCTION()
	void OnSensitivityChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UButton* ExitBtn;
};
