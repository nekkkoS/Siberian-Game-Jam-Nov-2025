// Copyright Offmeta

#include "CombineChipsPuzzleActor.h"

#include "Components/WidgetComponent.h"
#include "../../UI/Puzzle/SplittedImageWidget.h"

ACombineChipsPuzzleActor::ACombineChipsPuzzleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		SetRootComponent(RootComponent);
	}
	
	PuzzleChipsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PuzzleChipsWidgetComponent"));
	PuzzleChipsWidgetComponent->SetWidgetClass(SplittedImageWidgetClass);
	PuzzleChipsWidgetComponent->SetupAttachment(GetRootComponent());
}

void ACombineChipsPuzzleActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACombineChipsPuzzleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACombineChipsPuzzleActor::ShowRandomPuzzleChip()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowRandomPuzzleChip called in CombineChipsPuzzleActor."));
}
