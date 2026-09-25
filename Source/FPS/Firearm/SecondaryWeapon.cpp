// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS/Firearm/SecondaryWeapon.h"
#include "SecondaryWeapon.h"

ASecondaryWeapon::ASecondaryWeapon()
{
	Damage = 12.f;
	CurrentAmmo = MaxAmmo;
}

void ASecondaryWeapon::AddAmmo(int32 AmmoAmount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoAmount, 0, MaxAmmo);
}

bool ASecondaryWeapon::TryConsumeAmmo()
{
	if (CurrentAmmo <= 0) return false;
	CurrentAmmo--;
	return true;
}
