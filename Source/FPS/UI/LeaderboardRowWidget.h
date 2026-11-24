#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPS/GameMode/ArenaGameMode.h"
#include "LeaderboardRowWidget.generated.h"

class UTextBlock;
class UBorder;

UCLASS()
class FPS_API ULeaderboardRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupRow(int32 Rank, const FParticipantScore& ScoreData);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RankText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsText;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* RowBackground;
};