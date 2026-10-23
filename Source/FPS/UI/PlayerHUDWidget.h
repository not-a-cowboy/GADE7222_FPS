#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class AFirearmBase;
class UTexture2D;

UCLASS()
class FPS_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//HP Component
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPText;

	//Minialistic Health Bar
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthBar;

	//Weapon & Ammo Card ---
	//Picture/silhouette of the equipped weapon
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* WeaponIcon;

	//"∞" for Default Firearm, or "30 / 30" for Secondary Firearm
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	//Weapon Label
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WeaponNameText;

	//Mission Objectives / Match Timer
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ObjectiveText;

	//The Weapon Icons
	UPROPERTY(EditDefaultsOnly, Category = "UI|Icons")
	UTexture2D* DefaultWeaponTexture;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Icons")
	UTexture2D* SecondaryWeaponTexture;

	// Observer callbacks
	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleWeaponChanged(AFirearmBase* NewWeapon);

	UFUNCTION()
	void HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

private:
	UPROPERTY()
	AFirearmBase* ObservedWeapon;

	// Calculates Green -> Yellow/Orange -> Red based on 0.0 - 1.0 health ratio
	FLinearColor GetDynamicHealthColor(float HealthPercent) const;
};