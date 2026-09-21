#include "PlayerHealthComponent.h"

UPlayerHealthComponent::UPlayerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UPlayerHealthComponent::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth -= DamageAmount;

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

float UPlayerHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UPlayerHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UPlayerHealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentHealth / MaxHealth;
}

void UPlayerHealthComponent::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("PLAYER HAS DIED!"));
}