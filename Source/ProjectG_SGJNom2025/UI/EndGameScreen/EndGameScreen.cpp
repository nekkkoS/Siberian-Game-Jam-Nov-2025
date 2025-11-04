// Copyright Offmeta


#include "EndGameScreen.h"

#include "Components/Button.h"
#include "Components/Image.h"

void UEndGameScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackGroundImg)
		BackGroundImg->SetRenderOpacity(0.f);
	if(Image1)
		Image1->SetRenderOpacity(0.f);
	if(Image2)
		Image2->SetRenderOpacity(0.f);
	if(Image3)
		Image3->SetRenderOpacity(0.f);
	if(ExitBtn)
		ExitBtn->SetRenderOpacity(0.f);
}

void UEndGameScreen::PlayEndGameSequence()
{
	StepIndex = 0;

	// Сначала плавно показываем весь виджет (корень)
	SetRenderOpacity(0.f);
	SetVisibility(ESlateVisibility::Visible);

	// Используем таймер для плавного появления root
	FadeInWidget(this);

	// Запускаем таймер для пошаговой анимации
	GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, this, &UEndGameScreen::AnimateNextStep,
		FadeDuration + DelayBetweenElements, true);
}

void UEndGameScreen::AnimateNextStep()
{
	UWidget* WidgetToFade = nullptr;

	switch(StepIndex)
	{
	case 0: WidgetToFade = BackGroundImg; break;
	case 1: WidgetToFade = Image1; break;
	case 2: WidgetToFade = Image2; break;
	case 3: WidgetToFade = Image3; break;
	case 4: WidgetToFade = ExitBtn; break;
	default:
		GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);
		return;
	}

	if(WidgetToFade)
		FadeInWidget(WidgetToFade);

	StepIndex++;
}

void UEndGameScreen::FadeInWidget(UWidget* Widget)
{
	if(!Widget)
		return;

	FTimerHandle* Handle = new FTimerHandle();
	float TimeStep = 0.02f;
	float CurrentTime = 0.f;

	GetWorld()->GetTimerManager().SetTimer(*Handle, [Widget, Handle, TimeStep, this, CurrentTime]() mutable
	{
		CurrentTime += TimeStep;
		float Alpha = FMath::Clamp(CurrentTime / FadeDuration, 0.f, 1.f);
		Widget->SetRenderOpacity(Alpha);

		if(Alpha >= 1.f)
		{
		  GetWorld()->GetTimerManager().ClearTimer(*Handle);
		  delete Handle;
		}

	}, TimeStep, true);

}
