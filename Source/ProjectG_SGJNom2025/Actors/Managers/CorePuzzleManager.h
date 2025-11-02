// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorePuzzleManager.generated.h"

class UEyesightOverlayWidget;
DECLARE_MULTICAST_DELEGATE(FOnTriggerPuzzleChipSpawnSignature);

UCLASS()
class PROJECTG_SGJNOM2025_API ACorePuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	ACorePuzzleManager();
	virtual void Tick(float DeltaTime) override;

protected:
	FOnTriggerPuzzleChipSpawnSignature OnTriggerPuzzleChipSpawnDelegate;
	
	FTimerHandle PlayerInTriggerTimerHandle;
	FTimerHandle CheckIfCanSpawnPuzzleChipTimerHandle;

	TScriptInterface<class IBlinkingProviderInterface> BlinkingProviderInterface;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "PuzzleManager")
	TObjectPtr<AActor> PuzzleActorRef;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PuzzleManager")
	TWeakObjectPtr<class APlayableCharacter> PlayerCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PuzzleManager", meta = (ClampMin = 0.0f))
	float PlayerInTriggerTimerTickRate = 0.1f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PuzzleManager",
		meta = (ClampMax = 2.0f, ClampMin = 0.0f))
	float PlayerInTriggerTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PuzzleManager", meta = (ClampMin = 0.0f))
	float PlayerTriggerTimeAddition = 0.01f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PuzzleManager")
	bool bCanActivatePuzzleChipSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PuzzleManager", meta = (ClampMin = 0.0f))
	float TimeToPossibilityForPuzzleChipSpawn = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PuzzleManager", meta = (ClampMin = 0.0f))
	float CheckCanSpawnPuzzleChipTickRate = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PuzzleManager")
	TObjectPtr<class UBoxComponent> PuzzleBoxTrigger;

	virtual void BeginPlay() override;
	void StartCheckingIfCanSpawnPuzzleChip();

	UFUNCTION()
	void OnPuzzleBoxTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                    const FHitResult& SweepResult);

	UFUNCTION()
	void OnPuzzleBoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UFUNCTION()
	void OnEyesightWidgetReady(UEyesightOverlayWidget* EyesightWidget);
	
	UFUNCTION()
	void BlurCriticalThresholdReached();
};
