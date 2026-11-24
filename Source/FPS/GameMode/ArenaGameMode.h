#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class APlayerCharacter;
class UUserWidget;

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	InProgress,
	Paused,
	MatchOver
};

USTRUCT(BlueprintType)
struct FParticipantScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ParticipantName;

	UPROPERTY(BlueprintReadOnly)
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHumanPlayer = false;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor AccentColor = FLinearColor::White;

	UPROPERTY()
	AController* ControllerRef = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchTimeUpdated, float, RemainingSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, EMatchState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaderboardUpdated);

UCLASS()
class FPS_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	virtual void Tick(float DeltaTime) override;

	void RegisterParticipant(APlayerCharacter* Participant);

	// Match & Pause Controls
	UFUNCTION(BlueprintCallable, Category = "Match")
	void TogglePauseMatch();

	UFUNCTION(BlueprintCallable, Category = "Match")
	void ResumeMatch();

	UFUNCTION(BlueprintCallable, Category = "Match")
	void RestartMatch();

	UFUNCTION(BlueprintCallable, Category = "Match")
	void QuitToTitleScreen();

	// Getters for UI Widgets
	UFUNCTION(BlueprintPure, Category = "Match")
	TArray<FParticipantScore> GetSortedLeaderboard() const;

	UFUNCTION(BlueprintPure, Category = "Match")
	float GetRemainingMatchTime() const { return RemainingMatchTime; }

	UFUNCTION(BlueprintPure, Category = "Match")
	EMatchState GetCurrentMatchState() const { return CurrentMatchState; }

	// Observer Delegates
	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FOnMatchTimeUpdated OnMatchTimeUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FOnMatchStateChanged OnMatchStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FOnLeaderboardUpdated OnLeaderboardUpdated;

protected:
	virtual void BeginPlay() override;

	// 5-minute match duration (300 seconds) per Part 3 brief
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float MatchDurationSeconds = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	FName ArenaLevelName = TEXT("TheTest");

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	FName TitleLevelName = TEXT("TitleScreen");

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MatchOverWidgetClass;

	UPROPERTY()
	UUserWidget* ActivePauseMenu;

	UPROPERTY()
	UUserWidget* ActiveMatchOverScreen;

	UPROPERTY()
	TArray<AActor*> RespawnPoints;

	UPROPERTY()
	TArray<FParticipantScore> ParticipantScores;

	float RemainingMatchTime = 300.0f;
	EMatchState CurrentMatchState = EMatchState::InProgress;
	int32 BotCounter = 0;

	UFUNCTION()
	void HandleCharacterDeath(AActor* DeadActor, AController* InstigatedBy);

	void RespawnCharacter(APlayerCharacter* DeadCharacter);
	void AssignRandomStartingPositions();
	void EndMatch();
};