#include "FPS/GameMode/ArenaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "FPS/Player/PlayerCharacter.h"
#include "FPS/Health/PlayerHealthComponent.h"

AArenaGameMode::AArenaGameMode()
{
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Collect all TargetPoints in the level to use as random respawn locations (Requirement: >= 3)
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), RespawnPoints);

	// Subscribe to OnDeath for all characters currently in the arena
	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundCharacters);

	for (AActor* Actor : FoundCharacters)
	{
		if (APlayerCharacter* Character = Cast<APlayerCharacter>(Actor))
		{
			RegisterParticipant(Character);
		}
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
}

void AArenaGameMode::HandleCharacterDeath(AActor* DeadActor, AController* InstigatedBy)
{
	if (APlayerCharacter* DeadCharacter = Cast<APlayerCharacter>(DeadActor))
	{
		RespawnCharacter(DeadCharacter);
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
		const int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
		if (AActor* ChosenPoint = RespawnPoints[RandomIndex])
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