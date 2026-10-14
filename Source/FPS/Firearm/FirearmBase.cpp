#include "FPS/Firearm/FirearmBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AFirearmBase::AFirearmBase()
{
	FirearmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirearmMesh"));
	RootComponent = FirearmMesh;
	FirearmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	FVector TraceEnd = End;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		TraceEnd = Hit.ImpactPoint;
		if (AActor* HitActor = Hit.GetActor())
		{
			UGameplayStatics::ApplyDamage(
				HitActor,
				Damage,
				InstigatorPawn->GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}

	// Visual shot feedback required by the Part 1 brief
	FVector MuzzleStart = FirearmMesh ? FirearmMesh->GetComponentLocation() : Start;
	DrawDebugLine(GetWorld(), MuzzleStart, TraceEnd, TracerColor, false, 0.12f, 0, 1.5f);
}

AFirearmBase::~AFirearmBase()
{
}