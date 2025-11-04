// Copyright Offmeta

#include "PlayableCharacterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "../../../UI/Eyesight/EyesightOverlayWidget.h"
#include "../../../UI/PauseMenu/PauseMenuWidget.h"
#include "Components/AudioComponent.h"
#include "ProjectG_SGJNom2025/Character/Player/PlayableCharacter.h"

void APlayableCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(PlayerMappingContext, TEXT("In %s InputMappingContext is not set."), *GetActorLabel()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	// Виджет затемнения
	if (BlinkOverlayClass)
	{
		BlinkOverlay = CreateWidget<UUserWidget>(this, BlinkOverlayClass);
		checkf(BlinkOverlay, TEXT("Failed to create Widget"));
	}

	if (EyesightOverlayWidgetClass)
	{
		EyesightOverlayWidget = CreateWidget<UEyesightOverlayWidget>(this, EyesightOverlayWidgetClass);
		checkf(EyesightOverlayWidget, TEXT("Failed to create Widget"));
	}

	if (EyesightOverlayWidget && !EyesightOverlayWidget->IsInViewport())
	{
		EyesightOverlayWidget->AddToViewport();
		OnEyesightOverlayReadyDelegate.Broadcast(EyesightOverlayWidget);
	}
}

void APlayableCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
	                                   &APlayableCharacterPlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
	                                   &APlayableCharacterPlayerController::Look);
	EnhancedInputComponent->BindAction(BlinkAction, ETriggerEvent::Started, this,
									   &APlayableCharacterPlayerController::BlinkStart);
	EnhancedInputComponent->BindAction(BlinkAction, ETriggerEvent::Completed, this,
									   &APlayableCharacterPlayerController::BlinkEnd);
	EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Completed, this,
									   &APlayableCharacterPlayerController::OnPauseMenuToggle);
	EnhancedInputComponent->BindAction(SaveGameAction, ETriggerEvent::Started, this,
									   &APlayableCharacterPlayerController::OnSaveGame);
	EnhancedInputComponent->BindAction(LoadLevelAction, ETriggerEvent::Started, this,
									   &APlayableCharacterPlayerController::OnLoadLevel);
}

void APlayableCharacterPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

		PlayFootStepSound(ControlledPawn);
	}
}

void APlayableCharacterPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	AddYawInput(LookAxisVector.X * MouseSensitivity);
	AddPitchInput(LookAxisVector.Y * MouseSensitivity);
}

void APlayableCharacterPlayerController::PlayFootStepSound(APawn* ControlledPawn) const
{
	APlayableCharacter* PlayableChar = Cast<APlayableCharacter>(ControlledPawn);
	if (!PlayableChar)
		return;
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (PlayableChar->FootstepAudioComponent && PlayableChar->FootstepSound
		&& (CurrentTime - PlayableChar->LastFootstepTime) >= PlayableChar->FootstepInterval)
	{
		PlayableChar->FootstepAudioComponent->SetSound(PlayableChar->FootstepSound);
		PlayableChar->FootstepAudioComponent->Play();
		PlayableChar->LastFootstepTime = CurrentTime;
	}
}

void APlayableCharacterPlayerController::BlinkStart()
{
	if (EyesightOverlayWidget && !EyesightOverlayWidget->GetCanBlinkNow())
	{
		UE_LOG(LogTemp, Warning, TEXT("Blink ignored — blur not strong enough yet"));
		return;
	}
	
	if (!bIsBlinking && BlinkOverlay)
	{
		bIsBlinking = true;
		BlinkStartTime = GetWorld()->GetTimeSeconds();

		if (!BlinkOverlay->IsInViewport())
			BlinkOverlay->AddToViewport();

		if (EyesightOverlayWidget)
			EyesightOverlayWidget->HideBlinkHint();
	}
}

void APlayableCharacterPlayerController::BlinkEnd()
{
	if (bIsBlinking)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float ElapsedTime = CurrentTime - BlinkStartTime;

		if (ElapsedTime >= MinBlinkDuration)
		{
			if (BlinkOverlay && BlinkOverlay->IsInViewport())
				BlinkOverlay->RemoveFromParent();
			
			bIsBlinking = false;
			OnBlinkingEndedDelegate.Broadcast();
		}
		else
		{
			const float Delay = MinBlinkDuration - ElapsedTime;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (BlinkOverlay && BlinkOverlay->IsInViewport())
					BlinkOverlay->RemoveFromParent();
				
				bIsBlinking = false;
				OnBlinkingEndedDelegate.Broadcast();
			}, Delay, false);
		}
	}
}

FOnBlinkingEndedSignature& APlayableCharacterPlayerController::ProvideOnBlinkingEndedDelegate()
{
	return OnBlinkingEndedDelegate;
}

FOnEyesightOverlayReadySignature& APlayableCharacterPlayerController::ProvideOnEyesightOverlayReadyDelegate()
{
	return OnEyesightOverlayReadyDelegate;
}

UEyesightOverlayWidget* APlayableCharacterPlayerController::GetEyesightOverlayWidget_Implementation()
{
	return EyesightOverlayWidget;
}

void APlayableCharacterPlayerController::OnPauseMenuToggle()
{
	SetPause(true);

	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget)
	{
		PauseMenuWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void APlayableCharacterPlayerController::OnSaveGame()
{
	/*if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
	{
		SaveSubsystem->SaveGameData();

		if (SaveSubsystem->IsSaveGameExists("Save1", 0))
		{
			UE_LOG(LogTemp, Log, TEXT("Game saved successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to confirm save existence!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SaveSubsystem not found!"));
	}*/
}

void APlayableCharacterPlayerController::OnLoadLevel()
{
	/*if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
	{
		SaveSubsystem->LoadGameData();
		UE_LOG(LogTemp, Log, TEXT("Game load requested."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SaveSubsystem not found!"));
	}*/
}
