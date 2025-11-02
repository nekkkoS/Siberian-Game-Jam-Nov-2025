// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayableCharacter.generated.h"

UCLASS()
class PROJECTG_SGJNOM2025_API APlayableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayableCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerControllerRef;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSourceComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TObjectPtr<class UCameraComponent> CameraComponent;


	// ----- Звуки шагов -----

public:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	UAudioComponent* FootstepAudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* FootstepSound;

	// Интервал между шагами
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float FootstepInterval = 0.7f;

	// Время последнего шага
	float LastFootstepTime = 0.0f;

	
	// ----- Покачивания головой -----

private:

	void ExecuteHeadBob(float DeltaTime);
	
	// Амплитуда покачивания (в сантиметрах)
	UPROPERTY(EditDefaultsOnly, Category = "HeadBob")
	float BobAmplitude = 25.0f;

	// Частота покачивания
	UPROPERTY(EditDefaultsOnly, Category = "HeadBob")
	float BobFrequency = 10.0f;

	// Скорость сглаживания
	UPROPERTY(EditDefaultsOnly, Category = "HeadBob")
	float BobSmoothSpeed = 8.0f;

	// Скорость игрока при которой он начинает покачивать головой
	UPROPERTY(EditDefaultsOnly, Category = "HeadBob")
	float CharacterSpeedForHeadBob = 5.f;

	UPROPERTY()
	float BobTime = 0.0f;

	UPROPERTY()
	FVector DefaultCameraRelativeLocation;
};
