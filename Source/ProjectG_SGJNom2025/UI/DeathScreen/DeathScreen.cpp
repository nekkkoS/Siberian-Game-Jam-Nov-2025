// Copyright Offmeta


#include "DeathScreen.h"

#include "Components/Button.h"

void UDeathScreen::NativeConstruct()
{
	Super::NativeConstruct();

	RestartBtn->OnClicked.AddDynamic(this, &UDeathScreen::OnRestartBtnClicked);
}

void UDeathScreen::NativeDestruct()
{
	RestartBtn->OnClicked.RemoveDynamic(this, &UDeathScreen::OnRestartBtnClicked);
	
	Super::NativeDestruct();
}

void UDeathScreen::OnRestartBtnClicked()
{
	RemoveFromParent();
}
