#include "FPS/UI/MatchOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "FPS/UI/LeaderboardWidget.h"
#include "FPS/GameMode/ArenaGameMode.h"

void UMatchOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NewMatchButton)
	{
		NewMatchButton->OnClicked.RemoveDynamic(this, &UMatchOverWidget::OnNewMatchClicked);
		NewMatchButton->OnClicked.AddDynamic(this, &UMatchOverWidget::OnNewMatchClicked);
	}

	if (QuitToTitleButton)
	{
		QuitToTitleButton->OnClicked.RemoveDynamic(this, &UMatchOverWidget::OnQuitToTitleClicked);
		QuitToTitleButton->OnClicked.AddDynamic(this, &UMatchOverWidget::OnQuitToTitleClicked);
	}

	if (LeaderboardWidget)
	{
		LeaderboardWidget->RefreshLeaderboard();
	}

	if (WinnerText)
	{
		if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
		{
			const TArray<FParticipantScore> Scores = GM->GetSortedLeaderboard();
			if (Scores.Num() > 0)
			{
				WinnerText->SetText(FText::FromString(
					FString::Printf(TEXT("WINNER // %s (%d KILLS)"), *Scores[0].ParticipantName, Scores[0].Kills)
				));
			}
		}
	}
}

void UMatchOverWidget::OnNewMatchClicked()
{
	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->RestartMatch();
	}
}

void UMatchOverWidget::OnQuitToTitleClicked()
{
	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->QuitToTitleScreen();
	}
}