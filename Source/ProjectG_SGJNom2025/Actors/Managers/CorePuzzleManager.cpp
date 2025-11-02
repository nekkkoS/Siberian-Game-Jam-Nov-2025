// Copyright Offmeta

#include "CorePuzzleManager.h"

#include "../../Character/Player/PlayableCharacter.h"
#include "../../Interfaces/BlinkingProviderInterface.h"
#include "../../UI/Eyesight/EyesightOverlayWidget.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ACorePuzzleManager::ACorePuzzleManager()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		SetRootComponent(RootComponent);
	}

	PuzzleBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	PuzzleBoxTrigger->SetupAttachment(RootComponent);
}

void ACorePuzzleManager::BeginPlay()
{
	Super::BeginPlay();

	if (PuzzleBoxTrigger)
	{
		PuzzleBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACorePuzzleManager::OnPuzzleBoxTriggerStartOverlap);
		PuzzleBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ACorePuzzleManager::OnPuzzleBoxTriggerEndOverlap);
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	
	if (IBlinkingProviderInterface* BlinkingInterface = Cast<IBlinkingProviderInterface>(PlayerController))
	{
		BlinkingInterface->ProvideOnEyesightOverlayReadyDelegate().AddUObject(
			this, &ACorePuzzleManager::OnEyesightWidgetReady);
	}
}

void ACorePuzzleManager::StartCheckingIfCanSpawnPuzzleChip()
{
	if (!bCanActivatePuzzleChipSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("End check if can spawn puzzle chip."));
		GetWorld()->GetTimerManager().ClearTimer(CheckIfCanSpawnPuzzleChipTimerHandle);

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Start check if can spawn puzzle chip."));
}

void ACorePuzzleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACorePuzzleManager::OnPuzzleBoxTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                        bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Player entered puzzle trigger area."));
	PlayerCharacterRef = Cast<APlayableCharacter>(OtherActor);

	GetWorld()->GetTimerManager().SetTimer(
		PlayerInTriggerTimerHandle,
		[this]()
		{
			if (PlayerCharacterRef.IsValid())
			{
				PlayerInTriggerTime += PlayerTriggerTimeAddition;

				if (PlayerInTriggerTime >= TimeToPossibilityForPuzzleChipSpawn && PlayerInTriggerTimerHandle.IsValid())
				{
					bCanActivatePuzzleChipSpawn = true;
					GetWorld()->GetTimerManager().SetTimer(CheckIfCanSpawnPuzzleChipTimerHandle, this,
					                                       &ACorePuzzleManager::StartCheckingIfCanSpawnPuzzleChip,
					                                       CheckCanSpawnPuzzleChipTickRate, true);

					GetWorld()->GetTimerManager().ClearTimer(PlayerInTriggerTimerHandle);
				}
			}
		},
		PlayerInTriggerTimerTickRate,
		true
	);
}

void ACorePuzzleManager::OnPuzzleBoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerCharacterRef = nullptr;

	if (PlayerInTriggerTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerInTriggerTimerHandle);
	}
	bCanActivatePuzzleChipSpawn = false;
	PlayerInTriggerTime = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("Player exited puzzle trigger area."));
}

void ACorePuzzleManager::OnEyesightWidgetReady(UEyesightOverlayWidget* EyesightWidget)
{
	if (EyesightWidget)
	{
		EyesightWidget->OnBlurEffectCriticalThresholdReachedDelegate.AddUObject(
			this, &ACorePuzzleManager::BlurCriticalThresholdReached);
	}
}

void ACorePuzzleManager::BlurCriticalThresholdReached()
{
	// TODO: Handle spawn puzzle chip logic here.
	UE_LOG(LogTemp, Warning, TEXT("Blur critical threshold reached in Puzzle Manager."));
}
