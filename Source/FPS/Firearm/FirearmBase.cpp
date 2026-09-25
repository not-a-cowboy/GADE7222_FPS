#include "FPS/Firearm/FirearmBase.h"
#include "Kismet/GameplayStatics.h"

AFirearmBase::AFirearmBase()
{
	FirearmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirearmMesh"));
	RootComponent = FirearmMesh;
}

void AFirearmBase::Fire(APawn* InstigatorPawn)
{
	if (!InstigatorPawn || !TryConsumeAmmo())
	{
		return;
	}

	FVector Start;
	FRotator ViewRotation;
	InstigatorPawn->GetActorEyesViewPoint(Start, ViewRotation);
	FVector End = Start + (ViewRotation.Vector() * Range);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(InstigatorPawn);
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, InstigatorPawn->GetController(), this, UDamageType::StaticClass());
		}
	}
}

AFirearmBase::~AFirearmBase()
{
}
