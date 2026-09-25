#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "PlayerCharacter.generated.h"

class AFirearmBase;

UCLASS()
class FPS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditAnywhere)
	class UCameraComponent* PlayerCam;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	UPlayerHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	void MoveForward(float InputValue);
	void MoveRight(float InputValue);
	void TurnCamera(float InputValue);
	void LookUp(float InputValue);
	void Sprint();
	void StopSprinting();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AFirearmBase> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AFirearmBase> SecondaryWeaponClass;

	UPROPERTY() AFirearmBase* DefaultWeapon;
	UPROPERTY() AFirearmBase* SecondaryWeapon;
	UPROPERTY() AFirearmBase* CurrentWeapon;

	void EquipWeapon(AFirearmBase* NewWeapon);
	void SwitchToDefaultWeapon();
	void SwitchToSecondaryWeapon();
	void FireWeapon();

};
