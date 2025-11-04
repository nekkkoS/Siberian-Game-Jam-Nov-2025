// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/PuzzleChipProviderInterface.h"
#include "CombineChipsPuzzleActor.generated.h"

UCLASS()
class PROJECTG_SGJNOM2025_API ACombineChipsPuzzleActor : public AActor, public IPuzzleChipProviderInterface
{
	GENERATED_BODY()
	
	// When player blinks and blur threshold are met and player are in puzzle trigger.
	// One of the images becomes visible.

	// Call method from widget to show random puzzle chip.
	
public:
	ACombineChipsPuzzleActor();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PuzzleActor")
	TSubclassOf<class USplittedImageWidget> SplittedImageWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PuzzleActor")
	TObjectPtr<class UWidgetComponent> PuzzleChipsWidgetComponent;
	
	virtual void BeginPlay() override;
	virtual void ShowRandomPuzzleChip() override;
	virtual FOnAllChipsCombinedSignature& SubscribeToOnAllChipsCombinedDelegate() override;

private:
	FOnAllChipsCombinedSignature OnAllChipsCombinedDelegate;
};
