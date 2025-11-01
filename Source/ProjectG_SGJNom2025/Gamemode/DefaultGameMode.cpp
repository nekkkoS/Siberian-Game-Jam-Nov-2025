// Copyright Offmeta

#include "DefaultGameMode.h"

#include "../Character/Player/PlayableCharacter.h"
#include "ProjectG_SGJNom2025/Character/Player/Controller/PlayableCharacterPlayerController.h"

ADefaultGameMode::ADefaultGameMode()
{
	DefaultPawnClass = APlayableCharacter::StaticClass();
	PlayerControllerClass = APlayableCharacterPlayerController::StaticClass();
}
