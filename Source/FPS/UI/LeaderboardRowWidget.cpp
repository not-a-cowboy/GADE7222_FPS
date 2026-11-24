#include "FPS/UI/LeaderboardRowWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void ULeaderboardRowWidget::SetupRow(int32 Rank, const FParticipantScore& ScoreData)
{
	if (RankText)
	{
		RankText->SetText(FText::FromString(FString::Printf(TEXT("#%d"), Rank)));
	}

	if (NameText)
	{
		NameText->SetText(FText::FromString(ScoreData.ParticipantName));
	}

	if (KillsText)
	{
		KillsText->SetText(FText::FromString(FString::Printf(TEXT("%d KILLS"), ScoreData.Kills)));
	}

	if (ScoreData.bIsHumanPlayer)
	{
		const FLinearColor GoldHighlight(1.0f, 0.82f, 0.2f, 1.0f);
		if (RankText) RankText->SetColorAndOpacity(FSlateColor(GoldHighlight));
		if (NameText) NameText->SetColorAndOpacity(FSlateColor(GoldHighlight));
		if (KillsText) KillsText->SetColorAndOpacity(FSlateColor(GoldHighlight));
		if (RowBackground) RowBackground->SetBrushColor(FLinearColor(1.0f, 0.82f, 0.2f, 0.22f));
	}
	else
	{
		if (RankText) RankText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		if (NameText) NameText->SetColorAndOpacity(FSlateColor(ScoreData.AccentColor));
		if (KillsText) KillsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		if (RowBackground) RowBackground->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f));
	}
}