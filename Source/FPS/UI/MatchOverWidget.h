#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchOverWidget.generated.h"

class UButton;
class UTextBlock;
class ULeaderboardWidget;

UCLASS()
class FPS_API UMatchOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	ULeaderboardWidget* LeaderboardWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WinnerText;

	UPROPERTY(meta = (BindWidget))
	UButton* NewMatchButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitToTitleButton;

	UFUNCTION()
	void OnNewMatchClicked();

	UFUNCTION()
	void OnQuitToTitleClicked();
};