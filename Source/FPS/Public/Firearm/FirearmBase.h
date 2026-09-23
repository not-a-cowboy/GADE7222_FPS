// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirearmBase.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	AFirearmBase();
	virtual void Fire(APawn* InstigatorPawn);

	//~FirearmBase();
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Range = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	UStaticMeshComponent* FirearmMesh;

	virtual bool TryConsumeAmmo() { return true;}
};
