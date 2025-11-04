// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "n3mupySaveSettings.generated.h"

class USaveGame;

/**
 * 
 */
UCLASS(config=Game, DefaultConfig, meta = (DisplayName = "n3mupy Save Settings"))
class N3MUPYSAVESYSTEMPLUGIN_API Un3mupySaveSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	Un3mupySaveSettings();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "n3mupy | SaveSystem", 
		meta = (AllowAbstract = "false", 
			   ToolTip = "REQUIRED: Set your custom SaveGame class here. Must not be USaveGame!"))
	TSubclassOf<USaveGame> DefaultSaveGameClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category="n3mupy | SaveSystem")
	FString DefaultSlotName = "SaveSlot";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category="n3mupy | SaveSystem")
	int32 DefaultUserIndex = 0;

	UFUNCTION(BlueprintPure, Category = "Save System")
	static const Un3mupySaveSettings* Get();

#if WITH_EDITOR
	// Validation in edtior
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
