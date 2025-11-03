// Copyright Offmeta


#include "PlayableCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Controller/PlayableCharacterPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Subsystem/n3mupySaveSubsystem.h"
#include "ProjectG_SGJNom2025/SaveGame/DefaultSaveGame.h"

APlayableCharacter::APlayableCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->ProbeSize = 1.0f;
	SpringArmComponent->bDoCollisionTest = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(RootComponent);
	FootstepAudioComponent->bAutoActivate = false;

	DeathAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathAudioComponent"));
	DeathAudioComponent->SetupAttachment(RootComponent);
	DeathAudioComponent->bAutoActivate = false;
}

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ExecuteHeadBob(DeltaTime);
}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraComponent)
	{
		DefaultCameraRelativeLocation = CameraComponent->GetRelativeLocation();
	}
}

void APlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerControllerRef = Cast<APlayableCharacterPlayerController>(NewController);

	if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
	{
		SaveSubsystem->SubscribeToSaveSystem(
			this,
			&APlayableCharacter::OnSaveSubsystemReady,
			&APlayableCharacter::UseSavedData,
			&APlayableCharacter::AddDataForSave
		);
	}
}

void APlayableCharacter::ExecuteHeadBob(const float DeltaTime)
{
	const FVector Velocity = GetVelocity();
	const float Speed = FVector(Velocity.X, Velocity.Y, 0.f).Size();

	if (!CameraComponent)
		return;
	
	if (Speed > CharacterSpeedForHeadBob && GetCharacterMovement()->IsMovingOnGround())
	{
		// Движение — качаем голову
		BobTime += DeltaTime * BobFrequency;
		const float OffsetZ = FMath::Sin(BobTime) * BobAmplitude;
		const float OffsetY = FMath::Cos(BobTime * 0.5f) * (BobAmplitude * 0.3f);

		FVector TargetLocation = DefaultCameraRelativeLocation + FVector(0.f, OffsetY, OffsetZ);

		CameraComponent->SetRelativeLocation(
			FMath::VInterpTo(CameraComponent->GetRelativeLocation(), TargetLocation, DeltaTime, BobSmoothSpeed)
		);
	}
	else
	{
		// Стоим — возвращаем камеру на место
		BobTime = 0.f;
		CameraComponent->SetRelativeLocation(
			FMath::VInterpTo(CameraComponent->GetRelativeLocation(), DefaultCameraRelativeLocation, DeltaTime, BobSmoothSpeed)
		);
	}
}

void APlayableCharacter::SavePlayerState()
{
	if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
	{
		SaveSubsystem->SaveGameData(false);
		UE_LOG(LogTemp, Warning, TEXT("Manual SaveGameData() triggered"));
	}
}

void APlayableCharacter::OnSaveSubsystemReady()
{
	UE_LOG(LogTemp, Warning, TEXT("Save system is READY!"));
}

void APlayableCharacter::UseSavedData(USaveGame* SavedData)
{
	UDefaultSaveGame* Save = Cast<UDefaultSaveGame>(SavedData);
	if (!Save)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseSavedData: Cast failed"));
		return;
	}

	if (!Save->PlayerData.PlayerTransform.Equals(FTransform::Identity))
	{
		SetActorTransform(Save->PlayerData.PlayerTransform);
		if (AController* Ctrl = GetController())
		{
			Ctrl->SetControlRotation(Save->PlayerData.PlayerRotation);
		}

		UE_LOG(LogTemp, Warning, TEXT("Player transform loaded from save"));
	}
}

void APlayableCharacter::AddDataForSave(USaveGame* SavedData, bool bSuccess)
{
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDataForSave: Save failed!"));
		return;
	}

	UDefaultSaveGame* Save = Cast<UDefaultSaveGame>(SavedData);
	if (!Save)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDataForSave: Cast failed"));
		return;
	}
	
	Save->PlayerData.PlayerTransform = GetActorTransform();
	Save->PlayerData.PlayerRotation = GetControlRotation();

	UE_LOG(LogTemp, Warning, TEXT("Player data prepared for saving"));
}
void APlayableCharacter::Die()
{
	// Отключаем управление
	if (AController* Ctrl = GetController())
		Ctrl->DisableInput(Cast<APlayerController>(Ctrl));

	if (DeathAudioComponent && DeathSound)
	{
		DeathAudioComponent->SetSound(DeathSound);
		DeathAudioComponent->Play();
	}

	// Настраиваем таймер для показа виджета
	if (DeathWidgetClass)
	{
		GetWorld()->GetTimerManager().SetTimer(DeathWidgetTimerHandle, [this]()
		{
			if (!DeathWidget)
				DeathWidget = CreateWidget<UUserWidget>(GetWorld(), DeathWidgetClass);

			if (DeathWidget && !DeathWidget->IsInViewport())
			{
				DeathWidget->AddToViewport();

				// Можно добавить плавное появление через анимацию виджета или через SetRenderOpacity
			}
		}, 0.5f, false); // например, виджет появится через 0.5 сек после смерти
	}
}
