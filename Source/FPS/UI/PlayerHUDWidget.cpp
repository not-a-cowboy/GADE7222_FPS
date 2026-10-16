#include "FPS/UI/PlayerHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "FPS/Player/PlayerCharacter.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Firearm/FirearmBase.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ObjectiveText)
	{
		ObjectiveText->SetText(FText::FromString(TEXT("ACT II // ARENA DEATHMATCH")));
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if (UPlayerHealthComponent* HealthComp = Player->GetHealthComponent())
		{
			HealthComp->OnHealthChanged.AddDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);
			HandleHealthChanged(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
		}

		Player->OnWeaponChanged.AddDynamic(this, &UPlayerHUDWidget::HandleWeaponChanged);
		HandleWeaponChanged(Player->GetCurrentWeapon());
	}
}

FLinearColor UPlayerHUDWidget::GetDynamicHealthColor(float HealthPercent) const
{
	const FLinearColor GreenColor(0.15f, 0.95f, 0.35f, 1.0f);  // High HP
	const FLinearColor OrangeColor(1.0f, 0.65f, 0.1f, 1.0f);   // Mid HP 
	const FLinearColor RedColor(0.95f, 0.15f, 0.15f, 1.0f);    // Low HP

	if (HealthPercent >= 0.5f)
	{
		// Blend from Yellow-Orange (0.5) to Green (1.0)
		const float Alpha = (HealthPercent - 0.5f) / 0.5f;
		return FMath::Lerp(OrangeColor, GreenColor, Alpha);
	}
	else
	{
		// Blend from Red (0.0) to Yellow-Orange (0.5)
		const float Alpha = FMath::Clamp(HealthPercent / 0.5f, 0.0f, 1.0f);
		return FMath::Lerp(RedColor, OrangeColor, Alpha);
	}
}

void UPlayerHUDWidget::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	const float Percent = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
	const FLinearColor DynamicColor = GetDynamicHealthColor(Percent);

	if (HPText)
	{
		const int32 RoundedHP = FMath::CeilToInt(CurrentHealth);
		HPText->SetText(FText::FromString(FString::Printf(TEXT("HP | %d"), RoundedHP)));
		HPText->SetColorAndOpacity(FSlateColor(DynamicColor));
	}

	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
		HealthBar->SetFillColorAndOpacity(DynamicColor);
	}
}

void UPlayerHUDWidget::HandleWeaponChanged(AFirearmBase* NewWeapon)
{
	if (ObservedWeapon)
	{
		ObservedWeapon->OnAmmoChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleAmmoChanged);
	}

	ObservedWeapon = NewWeapon;
	if (!ObservedWeapon)
	{
		return;
	}

	const bool bIsInfiniteDefault = (ObservedWeapon->GetMaxAmmo() < 0);

	
	if (WeaponIcon)
	{
		UTexture2D* TargetIcon = bIsInfiniteDefault ? DefaultWeaponTexture : SecondaryWeaponTexture;
		if (TargetIcon)
		{
			WeaponIcon->SetBrushFromTexture(TargetIcon);
		}
	}

	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(bIsInfiniteDefault ? TEXT("DEFAULT") : TEXT("SECONDARY")));
	}

	ObservedWeapon->OnAmmoChanged.AddDynamic(this, &UPlayerHUDWidget::HandleAmmoChanged);
	HandleAmmoChanged(ObservedWeapon->GetCurrentAmmo(), ObservedWeapon->GetMaxAmmo());
}

void UPlayerHUDWidget::HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoText)
	{
		return;
	}

	if (MaxAmmo < 0)
	{
		// Infinity symbol for the Default Firearm
		AmmoText->SetText(FText::FromString(TEXT("\u221E")));
	}
	else
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}