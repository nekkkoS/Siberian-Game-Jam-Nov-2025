// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplittedImageWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API USplittedImageWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// TODO: Add 3 images, each representing a part of the whole image.
	// By default, all images are hidden.
	// By receiving delegate call, reveal one of image.
	// Method to reveal random image in this widget.
};
