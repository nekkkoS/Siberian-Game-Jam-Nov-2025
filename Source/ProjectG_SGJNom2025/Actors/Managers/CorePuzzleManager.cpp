// Copyright Offmeta

#include "CorePuzzleManager.h"

#include "../../Character/Player/PlayableCharacter.h"
#include "../../Interfaces/BlinkingProviderInterface.h"
#include "../../Interfaces/PuzzleChipProviderInterface.h"
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

	//PlayerControllerRef = UGameplayStatics::GetPlayerController(this, 0);

	/*if (IBlinkingProviderInterface* BlinkingInterface = Cast<IBlinkingProviderInterface>(PlayerControllerRef))
	{
		BlinkingInterface->ProvideOnEyesightOverlayReadyDelegate().
		AddUObject(this, &ACorePuzzleManager::OnEyesightWidgetReady);
		
		BlinkingInterface->ProvideOnBlinkingEndedDelegate()
		.AddUObject(this, &ACorePuzzleManager::OnBlinkEnded);
	}*/

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		BlinkingProviderInterface.SetObject(PC);
		BlinkingProviderInterface.SetInterface(Cast<IBlinkingProviderInterface>(PC));

		BlinkingProviderInterface->ProvideOnEyesightOverlayReadyDelegate()
		.AddUObject(this, &ACorePuzzleManager::OnEyesightWidgetReady);
		BlinkingProviderInterface->ProvideOnBlinkingEndedDelegate()
		.AddUObject(this, &ACorePuzzleManager::OnBlinkEnded);
	}
}

void ACorePuzzleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
					/*GetWorld()->GetTimerManager().SetTimer(CheckIfCanSpawnPuzzleChipTimerHandle, this,
					                                       &ACorePuzzleManager::StartCheckingIfCanSpawnPuzzleChip,
					                                       CheckCanSpawnPuzzleChipTickRate, true);*/

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

		bEyeSightWidgetReady = true;
	}
}

void ACorePuzzleManager::OnBlinkEnded()
{
	if (!bBlurThresholdReached || !PlayerCharacterRef.IsValid())
	{
		return;
	}
	
	if (IPuzzleChipProviderInterface* PuzzleChipProvider = Cast<IPuzzleChipProviderInterface>(PuzzleActorRef))
	{
		// Call method, when player ends blinking
		PuzzleChipProvider->ShowRandomPuzzleChip();
		// Also should reset local BlurThreshold local bool
		bBlurThresholdReached = false;
		
		// Also set bThreshold from widget to false here
		if (bEyeSightWidgetReady)
		{
			BlinkingProviderInterface->Execute_GetEyesightOverlayWidget(BlinkingProviderInterface.GetObject())
			->SetBlurEffectThresholdReached(false);
		}
	}
}

// Need to get bool value, that threshold is reached, and save it here.
void ACorePuzzleManager::BlurCriticalThresholdReached(bool bReached)
{
	// !NEED TESTS!
	// TODO: Blur threshold is reached, can spawn puzzle chip. Then wait again till blur threshold is reached again.
	// Add bool to reveal chip control.
	UE_LOG(LogTemp, Warning, TEXT("Blur critical threshold reached in Puzzle Manager."));

	// Here set BlurThreshold local bool to value received from delegate
	bBlurThresholdReached = bReached;
	UE_LOG(LogTemp, Warning, TEXT("bBlurThresholdReached in PuzzleManager: %s"), bBlurThresholdReached ? TEXT("true") : TEXT("false"));
}
