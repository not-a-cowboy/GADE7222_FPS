#include "FPS/Player/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Firearm/FirearmBase.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerCam = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCam->SetupAttachment(GetRootComponent());
	PlayerCam->bUsePawnControlRotation = true;

	WeaponMount = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponMount"));
	WeaponMount->SetupAttachment(PlayerCam);
	WeaponMount->SetRelativeLocation(FVector(45.0f, 25.0f, -20.0f));

	HealthComponent = CreateDefaultSubobject<UPlayerHealthComponent>(TEXT("HealthComponent"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Only spawn the HUD if controlled by a local human player (prevents Bots from spawning HUDs)
	if (IsPlayerControlled() && PlayerHUDClass)
	{
		if (UUserWidget* PlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass))
		{
			PlayerHUD->AddToViewport();
		}
	}

	FActorSpawnParameters Params;
	Params.Owner = this;

	if (DefaultWeaponClass)
	{
		DefaultWeapon = GetWorld()->SpawnActor<AFirearmBase>(DefaultWeaponClass, Params);
		if (DefaultWeapon)
		{
			DefaultWeapon->AttachToComponent(
				WeaponMount,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			EquipWeapon(DefaultWeapon);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnCamera", this, &APlayerCharacter::TurnCamera);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::FireWeapon);
	PlayerInputComponent->BindAction("Switch1", IE_Pressed, this, &APlayerCharacter::SwitchToDefaultWeapon);
	PlayerInputComponent->BindAction("Switch2", IE_Pressed, this, &APlayerCharacter::SwitchToSecondaryWeapon);
}

void APlayerCharacter::MoveForward(float InputValue)
{
	AddMovementInput(GetActorForwardVector(), InputValue);
}

void APlayerCharacter::MoveRight(float InputValue)
{
	AddMovementInput(GetActorRightVector(), InputValue);
}

void APlayerCharacter::TurnCamera(float InputValue)
{
	AddControllerYawInput(InputValue);
}

void APlayerCharacter::LookUp(float InputValue)
{
	AddControllerPitchInput(InputValue);
}

void APlayerCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::EquipWeapon(AFirearmBase* NewWeapon)
{
	if (!NewWeapon)
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	CurrentWeapon = NewWeapon;
	CurrentWeapon->SetActorHiddenInGame(false);
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void APlayerCharacter::SwitchToDefaultWeapon()
{
	EquipWeapon(DefaultWeapon);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	if (SecondaryWeapon)
	{
		EquipWeapon(SecondaryWeapon);
	}
}

void APlayerCharacter::FireWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire(this);
	}
}

void APlayerCharacter::PickupSecondaryWeapon(int32 AmmoAmount)
{
	CollectWeaponOrAmmo(AmmoAmount);
}

bool APlayerCharacter::CollectWeaponOrAmmo(int32 AmmoAmount)
{
	// Polymorphic call without casting to ASecondaryWeapon (LSP & OCP compliant)
	if (SecondaryWeapon)
	{
		return SecondaryWeapon->AddAmmo(AmmoAmount);
	}

	if (SecondaryWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		SecondaryWeapon = GetWorld()->SpawnActor<AFirearmBase>(SecondaryWeaponClass, Params);
		if (SecondaryWeapon)
		{
			SecondaryWeapon->AttachToComponent(
				WeaponMount,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);

			EquipWeapon(SecondaryWeapon);
			return true;
		}
	}

	return false;
}

bool APlayerCharacter::CollectHealth(float HealAmount)
{
	return HealthComponent ? HealthComponent->Heal(HealAmount) : false;
}

bool APlayerCharacter::NeedsAmmo() const
{
	if (!SecondaryWeapon)
	{
		return true; // Still needs to pick up the secondary weapon
	}
	return SecondaryWeapon->GetCurrentAmmo() < (SecondaryWeapon->GetMaxAmmo() / 2);
}

bool APlayerCharacter::NeedsHealth() const
{
	if (!HealthComponent)
	{
		return false;
	}
	return HealthComponent->GetCurrentHealth() < 75.0f;
}