#include "FPS/Pickup/WeaponPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "FPS/Interfaces/PickupCollectorInterface.h"

AWeaponPickup::AWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Automatically assign Unreal's built-in Cube so it is immediately visible in the level
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PickupMesh->SetStaticMesh(CubeMesh.Object);
		PickupMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->SetSphereRadius(120.0f);

	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::OnPickupOverlap);
	Tags.Add(TEXT("WeaponPickup"));
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
	if (!bIsActive || !OtherActor || OtherActor == this)
	{
		return;
	}

	if (IPickupCollectorInterface* Collector = Cast<IPickupCollectorInterface>(OtherActor))
	{
		if (Collector->CollectWeaponOrAmmo(AmmoAmount))
		{
			bIsActive = false;
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			GetWorldTimerManager().SetTimer(
				RespawnTimerHandle,
				this,
				&AWeaponPickup::RespawnPickup,
				RespawnTime,
				false
			);
		}
	}
}

void AWeaponPickup::RespawnPickup()
{
	bIsActive = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}