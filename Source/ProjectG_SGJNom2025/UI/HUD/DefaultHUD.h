// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DefaultHUD.generated.h"

class UEyesightOverlayWidget;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API ADefaultHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<APlayerController> PlayerControllerRef;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UEyesightOverlayWidget> EyesightOverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UEyesightOverlayWidget> EyesightOverlayWidget;

public:
	UFUNCTION(BlueprintCallable)
	UEyesightOverlayWidget* GetEyesightOverlay() { return EyesightOverlayWidget; } 
};
