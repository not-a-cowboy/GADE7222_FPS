// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS/Pickup/HealthPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "FPS/Interfaces/PickupCollectorInterface.h"

AHealthPickup::AHealthPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("PickupMesh")
    );

    RootComponent = PickupMesh;

    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder")
    );

    if (CylinderMesh.Succeeded())
    {
        PickupMesh->SetStaticMesh(CylinderMesh.Object);
        PickupMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
    }

    PickupCollision = CreateDefaultSubobject<USphereComponent>(
        TEXT("PickupCollision")
    );

    PickupCollision->SetupAttachment(RootComponent);
    PickupCollision->SetSphereRadius(120.0f);

    PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    PickupCollision->OnComponentBeginOverlap.AddDynamic(
        this,
        &AHealthPickup::OnPickupOverlap
    );

    Tags.Add(TEXT("HealthPickup"));
}

void AHealthPickup::BeginPlay()
{
    Super::BeginPlay();
}

void AHealthPickup::OnPickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!bIsActive || !OtherActor || OtherActor == this)
    {
        return;
    }

    if (IPickupCollectorInterface* Collector =
        Cast<IPickupCollectorInterface>(OtherActor))
    {
        if (Collector->CollectHealth(HealthAmount))
        {
            bIsActive = false;

            SetActorHiddenInGame(true);
            SetActorEnableCollision(false);

            GetWorldTimerManager().SetTimer(
                RespawnTimerHandle,
                this,
                &AHealthPickup::RespawnPickup,
                RespawnTime,
                false
            );
        }
    }
}

void AHealthPickup::RespawnPickup()
{
    bIsActive = true;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}