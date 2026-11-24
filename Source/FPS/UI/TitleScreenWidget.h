#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleScreenWidget.generated.h"

class UButton;
class UWidget;

UCLASS()
class FPS_API UTitleScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ToggleControlsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* ControlsInfoPanel;

	UPROPERTY(EditDefaultsOnly, Category = "TitleScreen")
	FName ArenaLevelName = TEXT("TheTest");

	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnQuitGameClicked();

	UFUNCTION()
	void OnToggleControlsClicked();
};