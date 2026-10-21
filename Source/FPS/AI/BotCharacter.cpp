#include "FPS/AI/BotCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "FPS/AI/BotAIController.h"
#include "FPS/GameMode/ArenaGameMode.h"

ABotCharacter::ABotCharacter()
{
	AIControllerClass = ABotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = true;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bUseControllerDesiredRotation = true;
		MoveComp->bOrientRotationToMovement = false;
	}


	BotBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BotBodyMesh"));
	BotBodyMesh->SetupAttachment(GetRootComponent());
	BotBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		BotBodyMesh->SetStaticMesh(CylinderMesh.Object);
		BotBodyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.75f));
	}
}

void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (BotBodyMesh)
	{
		UMaterialInstanceDynamic* DynMat = BotBodyMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(TEXT("BaseColor"), BotColor);
			DynMat->SetVectorParameterValue(TEXT("Color"), BotColor);
		}
	}

	if (AArenaGameMode* GM = GetWorld()->GetAuthGameMode<AArenaGameMode>())
	{
		GM->RegisterParticipant(this);
	}
}