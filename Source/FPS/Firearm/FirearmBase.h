#pragma once


#include "CoreMinimal.h"
#include "FirearmBase.generated.h"

UCLASS()
class FPS_API AFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	AFirearmBase();
	virtual void Fire(APawn* InstigatorPawn);

	~AFirearmBase();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Damage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	float Range = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Firearm")
	UStaticMeshComponent* FirearmMesh;

	virtual bool TryConsumeAmmo() {return true;}
};
