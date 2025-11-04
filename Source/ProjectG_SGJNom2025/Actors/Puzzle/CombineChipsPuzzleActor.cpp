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
	UE_LOG(LogTemp, Warning, TEXT("ACombineChipsPuzzleActor: ShowRandomPuzzleChip called in CombineChipsPuzzleActor."));
	if (Cast<USplittedImageWidget>(PuzzleChipsWidgetComponent->GetWidget())->RevealRandomImagePart() == 0)
	{
		// All parts revealed, puzzle completed.
		UE_LOG(LogTemp, Warning, TEXT("ACombineChipsPuzzleActor: All puzzle chips combined! Puzzle completed."));
		OnAllChipsCombinedDelegate.Broadcast();
	}
}

FOnAllChipsCombinedSignature& ACombineChipsPuzzleActor::SubscribeToOnAllChipsCombinedDelegate()
{
	return OnAllChipsCombinedDelegate;
}
