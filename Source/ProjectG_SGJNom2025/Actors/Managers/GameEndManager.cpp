// Copyright Offmeta

#include "GameEndManager.h"

#include "../../Interfaces/GameEndProviderInterface.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectG_SGJNom2025/Character/Player/Controller/PlayableCharacterPlayerController.h"
#include "ProjectG_SGJNom2025/UI/EndGameScreen/EndGameScreen.h"
#include "ProjectG_SGJNom2025/UI/Eyesight/EyesightOverlayWidget.h"

AGameEndManager::AGameEndManager()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		SetRootComponent(RootComponent);
	}

	GameEndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	GameEndTrigger->SetupAttachment(GetRootComponent());
}

void AGameEndManager::BeginPlay()
{
	Super::BeginPlay();

	if (GameEndTrigger)
	{
		GameEndTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGameEndManager::OnGameEndTriggerStartOverlap);
		GameEndTrigger->OnComponentEndOverlap.AddDynamic(this, &AGameEndManager::OnGameEndTriggerEndOverlap);
	}

	if (GameEndPuzzleManager)
	{
		GameEndProvider.SetObject(GameEndPuzzleManager);
		GameEndProvider.SetInterface(Cast<IGameEndProviderInterface>(GameEndPuzzleManager));
	}

	if (GameEndProvider)
	{
		GameEndProvider->SubscribeToOnGameEndDelegate().AddUObject(this, &AGameEndManager::OnGameEnd);
	}
}

void AGameEndManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameEndManager::OnGameEnd(bool bIsEnded)
{
	UE_LOG(LogTemp, Warning, TEXT("GameEndManager triggerred OnGameEnd()"));
	bIsGameEnded = bIsEnded;
}

void AGameEndManager::ShowGameEndWidget()
{
	APlayerController* PCtrl = UGameplayStatics::GetPlayerController(this, 0);
	if (!PCtrl)
		return;

	APlayableCharacterPlayerController* DefaultPCtrl = Cast<APlayableCharacterPlayerController>(PCtrl);
	if (!DefaultPCtrl)
		return;

	if (UEyesightOverlayWidget* EyesightWidget = DefaultPCtrl->GetEyesightOverlayWidget_Implementation())
	{
		EyesightWidget->StopBlurEffect();
	}
	
	if (!EndGameScreenWidget && EndGameScreenWidgetClass)
	{
		EndGameScreenWidget = CreateWidget<UEndGameScreen>(DefaultPCtrl, EndGameScreenWidgetClass);
	}

	if (!EndGameScreenWidget)
		return;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(EndGameScreenWidget->TakeWidget());
	DefaultPCtrl->SetInputMode(InputMode);
	DefaultPCtrl->bShowMouseCursor = true;
	
	EndGameScreenWidget->AddToViewport();
	EndGameScreenWidget->PlayEndGameSequence();

	if (EndGameMusic)
	{
		UGameplayStatics::PlaySound2D(this, EndGameMusic);
	}
}

void AGameEndManager::OnGameEndTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ShowGameEndWidget();
	
	if (!bIsGameEnded)
	{
		return;
	}

	ShowGameEndWidget();
}

void AGameEndManager::OnGameEndTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
