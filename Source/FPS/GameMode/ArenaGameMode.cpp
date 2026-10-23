
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

    // Find every Target Point placed in the arena.
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        ATargetPoint::StaticClass(),
        RespawnPoints
    );

    // Subscribe to death events for all characters in the level.
    TArray<AActor*> FoundCharacters;

    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        APlayerCharacter::StaticClass(),
        FoundCharacters
    );

    for (AActor* Actor : FoundCharacters)
    {
        if (APlayerCharacter* Character = Cast<APlayerCharacter>(Actor))
        {
            RegisterParticipant(Character);
        }
    }

    // Randomise the starting positions of the player and bots.
    AssignRandomStartingPositions();
}

void AArenaGameMode::AssignRandomStartingPositions()
{
    TArray<AActor*> FoundCharacters;

    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        APlayerCharacter::StaticClass(),
        FoundCharacters
    );

    if (RespawnPoints.Num() == 0)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Spawn system: No Target Points were found!")
        );
        return;
    }

    // Randomise the available spawn-point order.
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

        if (!Character)
        {
            continue;
        }

        // Stop if every Target Point has been used.
        if (SpawnIndex >= AvailablePoints.Num())
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Spawn system: Not enough Target Points for every character.")
            );
            break;
        }

        AActor* SpawnPoint = AvailablePoints[SpawnIndex];

        if (SpawnPoint)
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
                Controller->SetControlRotation(
                    SpawnPoint->GetActorRotation()
                );
            }

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("%s assigned to %s"),
                *GetNameSafe(Character),
                *GetNameSafe(SpawnPoint)
            );
        }

        ++SpawnIndex;
    }
}

void AArenaGameMode::RegisterParticipant(APlayerCharacter* Participant)
{
    if (!Participant)
    {
        return;
    }

    if (UPlayerHealthComponent* HealthComp =
        Participant->GetHealthComponent())
    {
        HealthComp->OnDeath.RemoveDynamic(
            this,
            &AArenaGameMode::HandleCharacterDeath
        );

        HealthComp->OnDeath.AddDynamic(
            this,
            &AArenaGameMode::HandleCharacterDeath
        );
    }
}

void AArenaGameMode::HandleCharacterDeath(
    AActor* DeadActor,
    AController* InstigatedBy)
{
    if (APlayerCharacter* DeadCharacter =
        Cast<APlayerCharacter>(DeadActor))
    {
        RespawnCharacter(DeadCharacter);
    }
}

void AArenaGameMode::RespawnCharacter(
    APlayerCharacter* DeadCharacter)
{
    if (!DeadCharacter)
    {
        return;
    }

    if (RespawnPoints.Num() > 0)
    {
        const int32 RandomIndex = FMath::RandRange(
            0,
            RespawnPoints.Num() - 1
        );

        if (AActor* ChosenPoint = RespawnPoints[RandomIndex])
        {
            DeadCharacter->SetActorLocationAndRotation(
                ChosenPoint->GetActorLocation(),
                ChosenPoint->GetActorRotation(),
                false,
                nullptr,
                ETeleportType::TeleportPhysics
            );

            if (AController* Controller =
                DeadCharacter->GetController())
            {
                Controller->SetControlRotation(
                    ChosenPoint->GetActorRotation()
                );
            }
        }
    }

    if (UPlayerHealthComponent* HealthComp =
        DeadCharacter->GetHealthComponent())
    {
        HealthComp->ResetHealth();
    }

    DeadCharacter->SwitchToDefaultWeapon();
}

