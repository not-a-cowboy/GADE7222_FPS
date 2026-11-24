#include "FPS/GameMode/ArenaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Blueprint/UserWidget.h"
#include "FPS/Player/PlayerCharacter.h"
#include "FPS/AI/BotCharacter.h"
#include "FPS/Health/PlayerHealthComponent.h"

AArenaGameMode::AArenaGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	RemainingMatchTime = MatchDurationSeconds;
	CurrentMatchState = EMatchState::InProgress;
	ParticipantScores.Empty();
	BotCounter = 0;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), RespawnPoints);

	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundCharacters);

	for (AActor* Actor : FoundCharacters)
	{
		if (APlayerCharacter* Character = Cast<APlayerCharacter>(Actor))
		{
			RegisterParticipant(Character);
		}
	}

	AssignRandomStartingPositions();
	OnMatchTimeUpdated.Broadcast(RemainingMatchTime);
	OnLeaderboardUpdated.Broadcast();
}

void AArenaGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentMatchState != EMatchState::InProgress)
	{
		return;
	}

	RemainingMatchTime = FMath::Max(0.0f, RemainingMatchTime - DeltaTime);
	OnMatchTimeUpdated.Broadcast(RemainingMatchTime);

	if (RemainingMatchTime <= 0.0f)
	{
		EndMatch();
	}
}

void AArenaGameMode::RegisterParticipant(APlayerCharacter* Participant)
{
	if (!Participant)
	{
		return;
	}

	if (UPlayerHealthComponent* HealthComp = Participant->GetHealthComponent())
	{
		HealthComp->OnDeath.RemoveDynamic(this, &AArenaGameMode::HandleCharacterDeath);
		HealthComp->OnDeath.AddDynamic(this, &AArenaGameMode::HandleCharacterDeath);
	}

	AController* Ctrl = Participant->GetController();
	const bool bAlreadyRegistered = ParticipantScores.ContainsByPredicate(
		[Ctrl](const FParticipantScore& Entry) { return Entry.ControllerRef == Ctrl; }
	);

	if (!bAlreadyRegistered && Ctrl)
	{
		FParticipantScore NewScore;
		NewScore.ControllerRef = Ctrl;
		NewScore.Kills = 0;

		if (Participant->IsPlayerControlled())
		{
			NewScore.ParticipantName = TEXT("PLAYER (YOU)");
			NewScore.bIsHumanPlayer = true;
			NewScore.AccentColor = FLinearColor(0.95f, 0.78f, 0.15f, 1.0f);
		}
		else
		{
			++BotCounter;
			NewScore.ParticipantName = FString::Printf(TEXT("BOT 0%d"), BotCounter);
			NewScore.bIsHumanPlayer = false;
			if (ABotCharacter* Bot = Cast<ABotCharacter>(Participant))
			{
				NewScore.AccentColor = Bot->BotColor;
			}
		}

		ParticipantScores.Add(NewScore);
	}
}

void AArenaGameMode::HandleCharacterDeath(AActor* DeadActor, AController* InstigatedBy)
{
	if (CurrentMatchState == EMatchState::MatchOver)
	{
		return;
	}

	APlayerCharacter* DeadCharacter = Cast<APlayerCharacter>(DeadActor);

	if (InstigatedBy && (!DeadCharacter || DeadCharacter->GetController() != InstigatedBy))
	{
		for (FParticipantScore& Entry : ParticipantScores)
		{
			if (Entry.ControllerRef == InstigatedBy)
			{
				Entry.Kills++;
				break;
			}
		}
		OnLeaderboardUpdated.Broadcast();
	}

	if (DeadCharacter)
	{
		RespawnCharacter(DeadCharacter);
	}
}

TArray<FParticipantScore> AArenaGameMode::GetSortedLeaderboard() const
{
	TArray<FParticipantScore> Sorted = ParticipantScores;
	Sorted.Sort([](const FParticipantScore& A, const FParticipantScore& B)
		{
			if (A.Kills != B.Kills)
			{
				return A.Kills > B.Kills;
			}
			return A.bIsHumanPlayer && !B.bIsHumanPlayer;
		});
	return Sorted;
}

void AArenaGameMode::TogglePauseMatch()
{
	if (CurrentMatchState == EMatchState::MatchOver)
	{
		return;
	}

	if (CurrentMatchState == EMatchState::Paused)
	{
		ResumeMatch();
	}
	else
	{
		CurrentMatchState = EMatchState::Paused;
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->SetShowMouseCursor(true);
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}

		if (PauseMenuWidgetClass && !ActivePauseMenu)
		{
			ActivePauseMenu = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
		}
		if (ActivePauseMenu && !ActivePauseMenu->IsInViewport())
		{
			ActivePauseMenu->AddToViewport(10);
		}

		OnLeaderboardUpdated.Broadcast();
		OnMatchStateChanged.Broadcast(CurrentMatchState);
	}
}

void AArenaGameMode::ResumeMatch()
{
	if (CurrentMatchState != EMatchState::Paused)
	{
		return;
	}

	CurrentMatchState = EMatchState::InProgress;
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (ActivePauseMenu && ActivePauseMenu->IsInViewport())
	{
		ActivePauseMenu->RemoveFromParent();
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	OnMatchStateChanged.Broadcast(CurrentMatchState);
}

void AArenaGameMode::EndMatch()
{
	CurrentMatchState = EMatchState::MatchOver;
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (ActivePauseMenu && ActivePauseMenu->IsInViewport())
	{
		ActivePauseMenu->RemoveFromParent();
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (MatchOverWidgetClass)
	{
		ActiveMatchOverScreen = CreateWidget<UUserWidget>(GetWorld(), MatchOverWidgetClass);
		if (ActiveMatchOverScreen)
		{
			ActiveMatchOverScreen->AddToViewport(20);
		}
	}

	OnLeaderboardUpdated.Broadcast();
	OnMatchStateChanged.Broadcast(CurrentMatchState);
}

void AArenaGameMode::RestartMatch()
{
	const FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevel));
}

void AArenaGameMode::QuitToTitleScreen()
{
	UGameplayStatics::OpenLevel(GetWorld(), TitleLevelName);
}

void AArenaGameMode::AssignRandomStartingPositions()
{
	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundCharacters);

	if (RespawnPoints.Num() == 0)
	{
		return;
	}

	TArray<AActor*> AvailablePoints = RespawnPoints;
	for (int32 i = AvailablePoints.Num() - 1; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		AvailablePoints.Swap(i, RandomIndex);
	}

	int32 SpawnIndex = 0;
	for (AActor* Actor : FoundCharacters)
	{
		APlayerCharacter* Character = Cast<APlayerCharacter>(Actor);
		if (!Character || SpawnIndex >= AvailablePoints.Num())
		{
			continue;
		}

		if (AActor* SpawnPoint = AvailablePoints[SpawnIndex])
		{
			Character->SetActorLocationAndRotation(
				SpawnPoint->GetActorLocation(),
				SpawnPoint->GetActorRotation(),
				false,
				nullptr,
				ETeleportType::TeleportPhysics
			);
			if (AController* Controller = Character->GetController())
			{
				Controller->SetControlRotation(SpawnPoint->GetActorRotation());
			}
		}
		++SpawnIndex;
	}
}

void AArenaGameMode::RespawnCharacter(APlayerCharacter* DeadCharacter)
{
	if (!DeadCharacter)
	{
		return;
	}

	if (RespawnPoints.Num() > 0)
	{
		TArray<AActor*> AllCharacters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllCharacters);

		TArray<AActor*> SafeSpawnPoints;
		AActor* FarthestFallbackPoint = RespawnPoints[0];
		float BestMinEnemyDistSq = -1.0f;
		const float MinSafeDistanceSq = FMath::Square(800.0f);

		for (AActor* Point : RespawnPoints)
		{
			if (!Point)
			{
				continue;
			}

			const FVector SpawnEyeLoc = Point->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f);
			bool bIsSafe = true;
			float ClosestEnemyDistSq = FLT_MAX;

			for (AActor* Actor : AllCharacters)
			{
				APlayerCharacter* OtherChar = Cast<APlayerCharacter>(Actor);
				if (!OtherChar || OtherChar == DeadCharacter)
				{
					continue;
				}

				if (OtherChar->GetHealthComponent() && OtherChar->GetHealthComponent()->GetCurrentHealth() <= 0.0f)
				{
					continue;
				}

				const float DistSq = FVector::DistSquared(SpawnEyeLoc, OtherChar->GetActorLocation());
				ClosestEnemyDistSq = FMath::Min(ClosestEnemyDistSq, DistSq);

				if (DistSq < MinSafeDistanceSq)
				{
					bIsSafe = false;
				}

				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Point);
				Params.AddIgnoredActor(OtherChar);
				Params.AddIgnoredActor(DeadCharacter);

				FHitResult Hit;
				const bool bBlockedByCover = GetWorld()->LineTraceSingleByChannel(
					Hit,
					OtherChar->GetPawnViewLocation(),
					SpawnEyeLoc,
					ECC_Visibility,
					Params
				);

				if (!bBlockedByCover)
				{
					bIsSafe = false;
				}
			}

			if (bIsSafe)
			{
				SafeSpawnPoints.Add(Point);
			}

			if (ClosestEnemyDistSq > BestMinEnemyDistSq)
			{
				BestMinEnemyDistSq = ClosestEnemyDistSq;
				FarthestFallbackPoint = Point;
			}
		}

		AActor* ChosenPoint = nullptr;
		if (SafeSpawnPoints.Num() > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, SafeSpawnPoints.Num() - 1);
			ChosenPoint = SafeSpawnPoints[RandomIndex];
		}
		else
		{
			ChosenPoint = FarthestFallbackPoint;
		}

		if (ChosenPoint)
		{
			DeadCharacter->SetActorLocationAndRotation(
				ChosenPoint->GetActorLocation(),
				ChosenPoint->GetActorRotation(),
				false,
				nullptr,
				ETeleportType::TeleportPhysics
			);

			if (AController* Controller = DeadCharacter->GetController())
			{
				Controller->SetControlRotation(ChosenPoint->GetActorRotation());
			}
		}
	}

	if (UPlayerHealthComponent* HealthComp = DeadCharacter->GetHealthComponent())
	{
		HealthComp->ResetHealth();
	}

	DeadCharacter->SwitchToDefaultWeapon();
}