
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class APlayerCharacter;

UCLASS()
class FPS_API AArenaGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AArenaGameMode();

    void RegisterParticipant(APlayerCharacter* Participant);

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    TArray<AActor*> RespawnPoints;

    UFUNCTION()
    void HandleCharacterDeath(AActor* DeadActor, AController* InstigatedBy);

    void RespawnCharacter(APlayerCharacter* DeadCharacter);

    void AssignRandomStartingPositions();
};