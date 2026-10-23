// Fill out your copyright notice in the Description page of Project Settings.
#include "FPS/Firearm/SecondaryWeapon.h"

ASecondaryWeapon::ASecondaryWeapon()
{
	Damage = 12.f;
	CurrentAmmo = MaxAmmo;
	TracerColor = FColor::Cyan;
}

bool ASecondaryWeapon::AddAmmo(int32 AmmoAmount)
{
	if (AmmoAmount <= 0 || CurrentAmmo >= MaxAmmo)
	{
		return false;
	}

	CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoAmount, 0, MaxAmmo);
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	return true;
}

bool ASecondaryWeapon::TryConsumeAmmo()
{
	if (CurrentAmmo <= 0)
	{
		return false;
	}

	CurrentAmmo--;
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	return true;
}