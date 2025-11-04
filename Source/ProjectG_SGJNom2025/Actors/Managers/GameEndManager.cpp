// Copyright Offmeta

#include "GameEndManager.h"

#include "../../Interfaces/GameEndProviderInterface.h"
#include "Components/BoxComponent.h"

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

void AGameEndManager::OnGameEndTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsGameEnded)
	{
		return;
	}

	// TODO: Show Game End Widget here.
	UE_LOG(LogTemp, Warning, TEXT("Should show Game End Widget now. (Widget is not impemented yet"));
}

void AGameEndManager::OnGameEndTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
