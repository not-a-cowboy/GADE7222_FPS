#include "FPS/UI/TitleScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (StartGameButton)
	{
		StartGameButton->OnClicked.RemoveDynamic(this, &UTitleScreenWidget::OnStartGameClicked);
		StartGameButton->OnClicked.AddDynamic(this, &UTitleScreenWidget::OnStartGameClicked);
	}

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.RemoveDynamic(this, &UTitleScreenWidget::OnQuitGameClicked);
		QuitGameButton->OnClicked.AddDynamic(this, &UTitleScreenWidget::OnQuitGameClicked);
	}

	if (ToggleControlsButton)
	{
		ToggleControlsButton->OnClicked.RemoveDynamic(this, &UTitleScreenWidget::OnToggleControlsClicked);
		ToggleControlsButton->OnClicked.AddDynamic(this, &UTitleScreenWidget::OnToggleControlsClicked);
	}

	if (ControlsInfoPanel)
	{
		ControlsInfoPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTitleScreenWidget::OnStartGameClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), ArenaLevelName);
}

void UTitleScreenWidget::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}

void UTitleScreenWidget::OnToggleControlsClicked()
{
	if (!ControlsInfoPanel)
	{
		return;
	}

	const bool bIsVisible = ControlsInfoPanel->IsVisible();
	ControlsInfoPanel->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}