#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardWidget.generated.h"

class UVerticalBox;
class ULeaderboardRowWidget;

UCLASS()
class FPS_API ULeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void RefreshLeaderboard();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* EntriesContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
	TSubclassOf<ULeaderboardRowWidget> RowWidgetClass;
};