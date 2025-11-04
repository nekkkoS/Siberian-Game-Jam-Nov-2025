// Copyright Offmeta

#include "SplittedImageWidget.h"
#include "Components/Image.h"

USplittedImageWidget::USplittedImageWidget(const FObjectInitializer& Object) : Super(Object)
{
	
}

void USplittedImageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ImagePart1->SetVisibility(ESlateVisibility::Hidden);
	ImagePart2->SetVisibility(ESlateVisibility::Hidden);
	ImagePart3->SetVisibility(ESlateVisibility::Hidden);

	ImagesToReveal.Add(ImagePart1);
	ImagesToReveal.Add(ImagePart2);
	ImagesToReveal.Add(ImagePart3);

	RemainingImagesCount = ImagesToReveal.Num();
}

int32 USplittedImageWidget::RevealRandomImagePart()
{
	if (ImagesToReveal.IsEmpty())
	{
		return 0;
	}

	const int32 RandomIndex = FMath::RandRange(0, ImagesToReveal.Num() - 1);
	if (UImage* RandomImage = ImagesToReveal[RandomIndex])
	{
		RandomImage->SetVisibility(ESlateVisibility::Visible);
		ImagesToReveal.RemoveAt(RandomIndex);
		--RemainingImagesCount;
		
		return RemainingImagesCount;
	}

	return 0;
}
