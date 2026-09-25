#include "FPS/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Firearm/FirearmBase.h"
#include "FPS/Firearm/SecondaryWeapon.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerCam = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCam->SetupAttachment(GetRootComponent());
	PlayerCam->bUsePawnControlRotation = true;
	HealthComponent = CreateDefaultSubobject<UPlayerHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerHUDClass)
	{
		UUserWidget* PlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
		}
	}
	
	FActorSpawnParameters Params;
	Params.Owner = this;

	if (DefaultWeaponClass)
	{
		DefaultWeapon = GetWorld()->SpawnActor<AFirearmBase>(DefaultWeaponClass, Params);
		DefaultWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	}
	EquipWeapon(DefaultWeapon);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
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
	FVector ForwardDirection = GetActorForwardVector();
	AddMovementInput(ForwardDirection, InputValue);
}	

void APlayerCharacter::MoveRight(float InputValue)
{
	FVector RightDirection = GetActorRightVector();
	AddMovementInput(RightDirection, InputValue);
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
	if (!NewWeapon) return;
	if (CurrentWeapon) CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = NewWeapon;
	CurrentWeapon->SetActorHiddenInGame(false);
}

void APlayerCharacter::SwitchToDefaultWeapon()
{
	EquipWeapon(DefaultWeapon);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	if (SecondaryWeapon) EquipWeapon(SecondaryWeapon);
}

void APlayerCharacter::FireWeapon()
{
	if (CurrentWeapon) CurrentWeapon->Fire(this);
}

void APlayerCharacter::PickupSecondaryWeapon(int32 AmmoAmount)
{
	if (SecondaryWeapon)
	{
		ASecondaryWeapon* Weapon = Cast<ASecondaryWeapon>(SecondaryWeapon);

		if (Weapon)
		{
			Weapon->AddAmmo(AmmoAmount);
		}

		return;
	}

	if (SecondaryWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		SecondaryWeapon = GetWorld()->SpawnActor<AFirearmBase>(SecondaryWeaponClass, Params);

		if (SecondaryWeapon)
		{
			SecondaryWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("WeaponSocket")
			);

			EquipWeapon(SecondaryWeapon);
		}
	}
}
