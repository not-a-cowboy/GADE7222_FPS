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

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPText;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* WeaponIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WeaponNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ObjectiveText;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Icons")
	UTexture2D* DefaultWeaponTexture;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Icons")
	UTexture2D* SecondaryWeaponTexture;

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleWeaponChanged(AFirearmBase* NewWeapon);

	UFUNCTION()
	void HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION()
	void HandleMatchTimeUpdated(float RemainingSeconds);

private:
	UPROPERTY()
	AFirearmBase* ObservedWeapon;

	FLinearColor GetDynamicHealthColor(float HealthPercent) const;
};