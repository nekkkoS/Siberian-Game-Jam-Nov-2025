// Copyright Offmeta


#include "n3mupySaveSettings.h"

#include "GameFramework/SaveGame.h"

Un3mupySaveSettings::Un3mupySaveSettings()
{
}

const Un3mupySaveSettings* Un3mupySaveSettings::Get()
{
	return GetDefault<Un3mupySaveSettings>();
}

#if WITH_EDITOR
void Un3mupySaveSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(Un3mupySaveSettings, DefaultSaveGameClass))
	{
		if (DefaultSaveGameClass == USaveGame::StaticClass())
		{
			UE_LOG(LogTemp, Error, 
				TEXT("DefaultSaveGameClass cannot be the base USaveGame class! Please select a custom SaveGame class."));
            
			// Reset to nullptr so that the user explicitly selects a class.
			DefaultSaveGameClass = nullptr;
		}
		else if (DefaultSaveGameClass && DefaultSaveGameClass->HasAnyClassFlags(CLASS_Abstract))
		{
			UE_LOG(LogTemp, Error, 
				TEXT("DefaultSaveGameClass cannot be an abstract class! Please select a concrete SaveGame class."));
            
			DefaultSaveGameClass = nullptr;
		}
	}
}
#endif
