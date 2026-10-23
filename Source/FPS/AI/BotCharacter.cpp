#include "FPS/AI/BotCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "FPS/AI/BotAIController.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
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
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawSize(FVector2D(100.0f, 15.0f));
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

	if (HealthComponent && HealthBarComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(
			this,
			&ABotCharacter::UpdateBotHealthBar
		);
	}


}
void ABotCharacter::UpdateBotHealthBar(float CurrentHealth, float MaxHealth)
{
	if (!HealthBarComponent || MaxHealth <= 0.0f)
	{
		return;
	}

	UUserWidget* Widget = HealthBarComponent->GetUserWidgetObject();

	if (!Widget)
	{
		return;
	}

	UProgressBar* HealthBar = Cast<UProgressBar>(
		Widget->GetWidgetFromName(TEXT("HealthProgressBar"))
	);

	if (HealthBar)
	{
		HealthBar->SetPercent(
			FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f)
		);
	}
}