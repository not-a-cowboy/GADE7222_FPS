#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponPickup.generated.h"

UCLASS()
class FPS_API AWeaponPickup : public AActor
{
	GENERATED_BODY()

public:
	AWeaponPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	class USphereComponent* PickupCollision;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int32 AmmoAmount = 30;

	UFUNCTION()
	void OnPickupOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};