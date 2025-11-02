// Copyright Offmeta

#include "DefaultGameMode.h"

#include "../Character/Player/PlayableCharacter.h"
#include "../Character/Player/Controller/PlayableCharacterPlayerController.h"
#include "../UI/HUD/DefaultHUD.h"

ADefaultGameMode::ADefaultGameMode()
{
	DefaultPawnClass = APlayableCharacter::StaticClass();
	PlayerControllerClass = APlayableCharacterPlayerController::StaticClass();
	HUDClass = ADefaultHUD::StaticClass();
}
