#include "FPS/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

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
