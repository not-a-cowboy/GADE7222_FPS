#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

UCLASS()
class FPS_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	void RegisterParticipant(class APlayerCharacter* Participant);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> RespawnPoints;

	UFUNCTION()
	void HandleCharacterDeath(AActor* DeadActor, AController* InstigatedBy);

	void RespawnCharacter(class APlayerCharacter* DeadCharacter);
};