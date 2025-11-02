// Copyright Offmeta

#include "PlayableCharacterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "../../../UI/Eyesight/EyesightOverlayWidget.h"

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
	}
}

void APlayableCharacterPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Add yaw and pitch input to the controller
		const float ScaledX = LookAxisVector.X;
		const float ScaledY = LookAxisVector.Y;

		AddYawInput(ScaledX);
		AddPitchInput(ScaledY);
	}
}

void APlayableCharacterPlayerController::BlinkStart()
{
	if (!bIsBlinking && BlinkOverlay)
	{
		bIsBlinking = true;
		BlinkStartTime = GetWorld()->GetTimeSeconds();

		if (!BlinkOverlay->IsInViewport())
			BlinkOverlay->AddToViewport();
		
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
