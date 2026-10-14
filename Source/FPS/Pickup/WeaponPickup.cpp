#include "FPS/Pickup/WeaponPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FPS/Interfaces/PickupCollectorInterface.h"

AWeaponPickup::AWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->SetSphereRadius(100.0f);

	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::OnPickupOverlap);
}

void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponPickup::OnPickupOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// Interface Pattern: Decoupled from concrete APlayerCharacter / ABotCharacter classes
	if (IPickupCollectorInterface* Collector = Cast<IPickupCollectorInterface>(OtherActor))
	{
		Collector->CollectWeaponOrAmmo(AmmoAmount);
	}
}