#include "FPS/UI/PauseMenuWidget.h"
#include "Components/Button.h"
#include "FPS/UI/LeaderboardWidget.h"
#include "FPS/GameMode/ArenaGameMode.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnResumeClicked);
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnRestartClicked);
		RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	}

	if (QuitToTitleButton)
	{
		QuitToTitleButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnQuitToTitleClicked);
		QuitToTitleButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitToTitleClicked);
	}

	if (LeaderboardWidget)
	{
		LeaderboardWidget->RefreshLeaderboard();
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->ResumeMatch();
	}
}

void UPauseMenuWidget::OnRestartClicked()
{
	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->RestartMatch();
	}
}

void UPauseMenuWidget::OnQuitToTitleClicked()
{
	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->QuitToTitleScreen();
	}
}