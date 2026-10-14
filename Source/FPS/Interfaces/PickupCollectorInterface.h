// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupCollectorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupCollectorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_API IPickupCollectorInterface
{
	GENERATED_BODY()

public:
	virtual bool CollectWeaponOrAmmo(int32 AmmoAmount) = 0;
	virtual bool CollectHealth(float HealAmount) = 0;
	virtual bool NeedsAmmo() const = 0;
	virtual bool NeedsHealth() const = 0;
public:
};
