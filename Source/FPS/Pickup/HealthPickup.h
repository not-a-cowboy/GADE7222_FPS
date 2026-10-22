#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "TimerManager.h"
#include "HealthPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class FPS_API AHealthPickup : public AActor
{
    GENERATED_BODY()

public:
    AHealthPickup();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* PickupMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    USphereComponent* PickupCollision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    float HealthAmount = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    float RespawnTime = 10.0f;

    bool bIsActive = true;

    FTimerHandle RespawnTimerHandle;

    UFUNCTION()
    void OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void RespawnPickup();
};