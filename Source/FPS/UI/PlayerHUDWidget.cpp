#include "FPS/UI/PlayerHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "FPS/Player/PlayerCharacter.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Firearm/FirearmBase.h"
#include "FPS/GameMode/ArenaGameMode.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->OnMatchTimeUpdated.AddDynamic(this, &UPlayerHUDWidget::HandleMatchTimeUpdated);
		HandleMatchTimeUpdated(GM->GetRemainingMatchTime());
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

void UPlayerHUDWidget::HandleMatchTimeUpdated(float RemainingSeconds)
{
	if (!ObjectiveText)
	{
		return;
	}

	const int32 TotalSeconds = FMath::CeilToInt(RemainingSeconds);
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	ObjectiveText->SetText(FText::FromString(
		FString::Printf(TEXT("MATCH TIME // %02d:%02d"), Minutes, Seconds)
	));

	if (TotalSeconds <= 30)
	{
		ObjectiveText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.2f, 0.2f, 1.0f)));
	}
	else
	{
		ObjectiveText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
}

FLinearColor UPlayerHUDWidget::GetDynamicHealthColor(float HealthPercent) const
{
	const FLinearColor GreenColor(0.15f, 0.95f, 0.35f, 1.0f);
	const FLinearColor OrangeColor(1.0f, 0.65f, 0.1f, 1.0f);
	const FLinearColor RedColor(0.95f, 0.15f, 0.15f, 1.0f);

	if (HealthPercent >= 0.5f)
	{
		const float Alpha = (HealthPercent - 0.5f) / 0.5f;
		return FMath::Lerp(OrangeColor, GreenColor, Alpha);
	}
	else
	{
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
			WeaponIcon->SetBrushFromTexture(TargetIcon, true);
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
		AmmoText->SetText(FText::FromString(TEXT("\u221E")));
	}
	else
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}