// Copyright Offmeta


#include "PlayableCharacter.h"

#include "Camera/CameraComponent.h"
#include "Controller/PlayableCharacterPlayerController.h"
#include "GameFramework/SpringArmComponent.h"

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
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerControllerRef = Cast<APlayableCharacterPlayerController>(NewController);
}

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
