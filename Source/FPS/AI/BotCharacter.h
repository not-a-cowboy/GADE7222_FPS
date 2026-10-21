#pragma once

#include "CoreMinimal.h"
#include "FPS/Player/PlayerCharacter.h"
#include "BotCharacter.generated.h"

UCLASS()
class FPS_API ABotCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	ABotCharacter();

	// Editable per-instance color in the Level Editor so all 4+ bots are visually distinct (5 Marks)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot|Appearance")
	FLinearColor BotColor = FLinearColor::Red;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Bot|Appearance")
	UStaticMeshComponent* BotBodyMesh;
};