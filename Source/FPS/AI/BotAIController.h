#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BotAIController.generated.h"

class APlayerCharacter;

UENUM(BlueprintType)
enum class EBotState : uint8
{
	Patrolling,
	EngagingTarget,
	InvestigatingLastSeen,
	SeekingHealth,
	SeekingAmmo,
	EvadingTarget
};

UCLASS()
class FPS_API ABotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABotAIController();

	UFUNCTION(BlueprintPure, Category = "AI")
	EBotState GetCurrentState() const { return CurrentState; }

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EBotState CurrentState = EBotState::Patrolling;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float SightRadius = 3000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float VisionAngleDegrees = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float FireInterval = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float AimInterpSpeed = 10.0f;

	UPROPERTY()
	APlayerCharacter* ControlledBot;

	UPROPERTY()
	APlayerCharacter* CurrentTarget;

	UPROPERTY()
	AActor* VisibleHealthPickup;

	UPROPERTY()
	AActor* VisibleWeaponPickup;

	// Object Permanence (Smart AI Target Memory)
	FVector LastKnownTargetLocation = FVector::ZeroVector;
	bool bHasLastKnownTarget = false;
	float LastSeenTargetTime = -10.0f;

	float LastFireTime = 0.0f;
	float LastDecisionTime = 0.0f;
	float LastStrafeTime = 0.0f;
	float LastDamagedTime = -10.0f;
	float PreviousHealth = 100.0f;

	void UpdatePerception();
	void EvaluatePriorities();
	void ExecuteCombat(float DeltaTime);
	void FaceMovementDirection(float DeltaTime);
	void FleeToCoverOrSafety();
	void PatrolArena();

	bool CanSeeActor(AActor* TargetActor, bool bIgnoreFOV = false) const;
	bool IsPickupAvailable(AActor* PickupActor) const;
	bool IsHealthPickupActor(AActor* Actor) const;
	bool IsWeaponPickupActor(AActor* Actor) const;

	UFUNCTION()
	void HandleBotHealthChanged(float CurrentHealth, float MaxHealth);
};