// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPS/Firearm/FirearmBase.h"
#include "SecondaryWeapon.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API ASecondaryWeapon : public AFirearmBase
{
	GENERATED_BODY()
	
public:
	ASecondaryWeapon();

	UFUNCTION(BlueprintCallable)
	void AddAmmo(int32 AmmoAmount);
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

protected:
	virtual bool TryConsumeAmmo() override;

	UPROPERTY(EditDefaultsOnly) int32 MaxAmmo = 30;
	UPROPERTY(VisibleAnywhere) int32 CurrentAmmo;
};
