// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplittedImageWidget.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API USplittedImageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USplittedImageWidget(const FObjectInitializer& Object);
	
	UFUNCTION(BlueprintCallable)
	bool RevealRandomImagePart();
	
protected:
	virtual void NativeConstruct() override;
	
private:
	TArray<TObjectPtr<UImage>> ImagesToReveal;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImagePart1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImagePart2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImagePart3;
};
