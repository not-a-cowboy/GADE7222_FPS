#pragma once

#include "CoreMinimal.h"
#include "FPS/Firearm/FirearmBase.h"
#include "SecondaryWeapon.generated.h"

UCLASS()
class FPS_API ASecondaryWeapon : public AFirearmBase
{
	GENERATED_BODY()

public:
	ASecondaryWeapon();

	virtual bool AddAmmo(int32 AmmoAmount) override;
	virtual int32 GetCurrentAmmo() const override { return CurrentAmmo; }
	virtual int32 GetMaxAmmo() const override { return MaxAmmo; }
	virtual bool HasAmmo() const override { return CurrentAmmo > 0; }

protected:
	virtual bool TryConsumeAmmo() override;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	int32 MaxAmmo = 30;

	UPROPERTY(VisibleAnywhere, Category = "Firearm")
	int32 CurrentAmmo;
};