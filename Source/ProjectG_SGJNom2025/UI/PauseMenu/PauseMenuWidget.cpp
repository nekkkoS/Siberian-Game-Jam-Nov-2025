// Copyright Offmeta


#include "PauseMenuWidget.h"

#include "AudioDevice.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "ProjectG_SGJNom2025/Character/Player/Controller/PlayableCharacterPlayerController.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ResumeBtn->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	
	// Звук
	VolumeSlider->OnValueChanged.AddDynamic(this, &UPauseMenuWidget::OnVolumeChanged);
	float CurrentVolume = 1.0f;
	if (FAudioDeviceHandle AudioDevice = GetWorld()->GetAudioDevice())
		CurrentVolume = AudioDevice->GetTransientPrimaryVolume();
	
	VolumeSlider->SetValue(CurrentVolume);
	UE_LOG(LogTemp, Log, TEXT("Initial volume: %f"), CurrentVolume);
	
	// Чувствительность мыши
	SensitivitySlider->OnValueChanged.AddDynamic(this, &UPauseMenuWidget::OnSensitivityChanged);
	if (APlayableCharacterPlayerController* PC = Cast<APlayableCharacterPlayerController>(GetOwningPlayer()))
		SensitivitySlider->SetValue(PC->GetMouseSensitivity());
	else
	{
		SensitivitySlider->SetValue(1.0f);
		UE_LOG(LogTemp, Error, TEXT("Can't get PlayerController in UPauseMenuWidget::NativeConstruct"));
	}
}

void UPauseMenuWidget::NativeDestruct()
{
	ResumeBtn->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	VolumeSlider->OnValueChanged.RemoveDynamic(this, &UPauseMenuWidget::OnVolumeChanged);
	SensitivitySlider->OnValueChanged.RemoveDynamic(this, &UPauseMenuWidget::OnSensitivityChanged);
	
	Super::NativeDestruct();
}

void UPauseMenuWidget::OnResumeClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL"));
		return;
	}

	PC->SetPause(false);
	RemoveFromParent();
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
}

void UPauseMenuWidget::OnVolumeChanged(float Value)
{
	if (FAudioDeviceHandle AudioDevice = GetWorld()->GetAudioDevice())
	{
		AudioDevice->SetTransientPrimaryVolume(Value);
		UE_LOG(LogTemp, Log, TEXT("Global volume set to %f"), Value);
	}
}

void UPauseMenuWidget::OnSensitivityChanged(float Value)
{
	APlayableCharacterPlayerController* MyPC = Cast<APlayableCharacterPlayerController>(GetOwningPlayer());
	if (MyPC)
	{
		MyPC->SetMouseSensitivity(Value);
		UE_LOG(LogTemp, Log, TEXT("Sensitivity changed: %f"), Value);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Can't get PlayerController in UPauseMenuWidget::OnSensitivityChanged"));
}
