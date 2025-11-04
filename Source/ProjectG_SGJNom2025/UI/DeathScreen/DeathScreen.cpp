// Copyright Offmeta


#include "DeathScreen.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDeathScreen::NativeConstruct()
{
	Super::NativeConstruct();

	RestartBtn->OnClicked.AddDynamic(this, &UDeathScreen::OnRestartBtnClicked);

	if (ShowRestartBtnAnim)
	{
		PlayAnimation(ShowRestartBtnAnim);
	}
}

void UDeathScreen::NativeDestruct()
{
	RestartBtn->OnClicked.RemoveDynamic(this, &UDeathScreen::OnRestartBtnClicked);
	
	Super::NativeDestruct();
}

void UDeathScreen::OnRestartBtnClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	// Скрываем курсор и сбрасываем ввод, чтобы вернуть управление после перезапуска
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());

	const FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}
