// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreen.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UDeathScreen : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartBtn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowRestartBtnAnim;

	UFUNCTION()
	void OnRestartBtnClicked();
};
