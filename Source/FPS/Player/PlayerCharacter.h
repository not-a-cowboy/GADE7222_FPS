#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Interfaces/PickupCollectorInterface.h"
#include "PlayerCharacter.generated.h"

class AFirearmBase;
class UUserWidget;
class FPlayerMovementState;
class FWalkingMovementState;
class FSprintingMovementState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, AFirearmBase*, CurrentWeapon);

enum class EPlayerMovementState : uint8
{
	Walking,
	Sprinting
};

UCLASS()
class FPS_API APlayerCharacter : public ACharacter, public IPickupCollectorInterface
{
	GENERATED_BODY()

	friend class FWalkingMovementState;
	friend class FSprintingMovementState;

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool CollectWeaponOrAmmo(int32 AmmoAmount) override;
	virtual bool CollectHealth(float HealAmount) override;
	virtual bool NeedsAmmo() const override;
	virtual bool NeedsHealth() const override;

	void Sprint();
	void StopSprinting();
	void EquipWeapon(AFirearmBase* NewWeapon);
	void SwitchToDefaultWeapon();
	void SwitchToSecondaryWeapon();
	void FireWeapon();
	void PickupSecondaryWeapon(int32 AmmoAmount);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AFirearmBase* GetSecondaryWeapon() const { return SecondaryWeapon; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AFirearmBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Health")
	UPlayerHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponChanged OnWeaponChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* PlayerCam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USceneComponent* WeaponMount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UPlayerHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	void MoveForward(float InputValue);
	void MoveRight(float InputValue);
	void TurnCamera(float InputValue);
	void LookUp(float InputValue);
	void TogglePause();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AFirearmBase> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AFirearmBase> SecondaryWeaponClass;

	UPROPERTY()
	AFirearmBase* DefaultWeapon;

	UPROPERTY()
	AFirearmBase* SecondaryWeapon;

	UPROPERTY()
	AFirearmBase* CurrentWeapon;

	EPlayerMovementState CurrentMovementState =
		EPlayerMovementState::Walking;

	FPlayerMovementState* MovementStateObject = nullptr;

	void SetMovementState(EPlayerMovementState NewState);
};