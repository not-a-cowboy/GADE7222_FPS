#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "FirearmBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS()
class FPS_API AFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	AFirearmBase();
	virtual void Fire(APawn* InstigatorPawn);

	~AFirearmBase();

	// Polymorphic ammo interface (satisfies Liskov Substitution Principle)
	virtual bool AddAmmo(int32 AmmoAmount) { return false; }
	virtual int32 GetCurrentAmmo() const { return -1; } // -1 = infinite ammo
	virtual int32 GetMaxAmmo() const { return -1; }
	virtual bool HasAmmo() const { return true; }

	UPROPERTY(BlueprintAssignable, Category = "Firearm|Events")
	FOnAmmoChanged OnAmmoChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Damage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Range = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	FColor TracerColor = FColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	UStaticMeshComponent* FirearmMesh;

	virtual bool TryConsumeAmmo() {return true;}
};
