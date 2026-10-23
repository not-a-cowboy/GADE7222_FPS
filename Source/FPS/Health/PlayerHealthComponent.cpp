#include "FPS/Health/PlayerHealthComponent.h"
#include "GameFramework/Actor.h"

UPlayerHealthComponent::UPlayerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	// Observer Pattern: Component listens to its Owner's damage events automatically
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UPlayerHealthComponent::HandleTakeAnyDamage);
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UPlayerHealthComponent::HandleTakeAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	TakeDamage(Damage, InstigatedBy);
}

void UPlayerHealthComponent::TakeDamage(float DamageAmount, AController* InstigatedBy)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die(InstigatedBy);
	}
}

bool UPlayerHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentHealth <= 0.0f || CurrentHealth >= MaxHealth)
	{
		return false;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	return true;
}

void UPlayerHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UPlayerHealthComponent::GetHealthPercent() const
{
	return (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
}

void UPlayerHealthComponent::Die(AController* KillerController)
{
	UE_LOG(LogTemp, Warning, TEXT("%s HAS DIED!"), *GetNameSafe(GetOwner()));
	OnDeath.Broadcast(GetOwner(), KillerController);
}