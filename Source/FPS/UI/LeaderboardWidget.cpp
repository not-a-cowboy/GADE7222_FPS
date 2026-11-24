#include "FPS/UI/LeaderboardWidget.h"
#include "Components/VerticalBox.h"
#include "FPS/UI/LeaderboardRowWidget.h"
#include "FPS/GameMode/ArenaGameMode.h"

void ULeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->OnLeaderboardUpdated.RemoveDynamic(this, &ULeaderboardWidget::RefreshLeaderboard);
		GM->OnLeaderboardUpdated.AddDynamic(this, &ULeaderboardWidget::RefreshLeaderboard);
	}

	RefreshLeaderboard();
}

void ULeaderboardWidget::RefreshLeaderboard()
{
	if (!EntriesContainer || !RowWidgetClass)
	{
		return;
	}

	EntriesContainer->ClearChildren();

	AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>();
	if (!GM)
	{
		return;
	}

	const TArray<FParticipantScore> SortedScores = GM->GetSortedLeaderboard();
	for (int32 i = 0; i < SortedScores.Num(); ++i)
	{
		if (ULeaderboardRowWidget* Row = CreateWidget<ULeaderboardRowWidget>(this, RowWidgetClass))
		{
			Row->SetupRow(i + 1, SortedScores[i]);
			EntriesContainer->AddChildToVerticalBox(Row);
		}
	}
}