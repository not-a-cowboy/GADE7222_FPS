#include "FPS/AI/BotAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "FPS/Player/PlayerCharacter.h"
#include "FPS/Health/PlayerHealthComponent.h"
#include "FPS/Firearm/FirearmBase.h"

class FBotState
{
public:
	virtual ~FBotState() = default;
	virtual void Execute(ABotAIController& Controller) = 0;
};

class FPatrollingBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->StopSprinting();
		Controller.PatrolArena();
	}
};

class FEngagingBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->StopSprinting();

		if (AFirearmBase* SecWeapon =
			Controller.ControlledBot->GetSecondaryWeapon())
		{
			if (SecWeapon->HasAmmo())
			{
				Controller.ControlledBot->SwitchToSecondaryWeapon();
			}
			else
			{
				Controller.ControlledBot->SwitchToDefaultWeapon();
			}
		}
		else
		{
			Controller.ControlledBot->SwitchToDefaultWeapon();
		}

		Controller.ExecuteCombatMovement();
	}
};

class FEvadingBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->Sprint();

		if (Controller.VisibleHealthPickup)
		{
			Controller.MoveToActor(Controller.VisibleHealthPickup, 15.0f);
		}
		else
		{
			Controller.FleeToCoverOrSafety();
		}
	}
};

class FSeekingHealthBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->StopSprinting();
		Controller.MoveToActor(Controller.VisibleHealthPickup, 15.0f);
	}
};

class FSeekingAmmoBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->StopSprinting();
		Controller.MoveToActor(Controller.VisibleWeaponPickup, 15.0f);
	}
};

class FInvestigatingBotState : public FBotState
{
public:
	virtual void Execute(ABotAIController& Controller) override
	{
		Controller.ControlledBot->StopSprinting();

		Controller.MoveToLocation(Controller.LastKnownTargetLocation, 50.0f);

		if (FVector::DistSquared2D(
			Controller.ControlledBot->GetActorLocation(),
			Controller.LastKnownTargetLocation) < FMath::Square(120.0f))
		{
			Controller.bHasLastKnownTarget = false;
		}
	}
};

ABotAIController::ABotAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledBot = Cast<APlayerCharacter>(InPawn);
	if (ControlledBot && ControlledBot->GetHealthComponent())
	{
		PreviousHealth = ControlledBot->GetHealthComponent()->GetCurrentHealth();
		ControlledBot->GetHealthComponent()->OnHealthChanged.RemoveDynamic(this, &ABotAIController::HandleBotHealthChanged);
		ControlledBot->GetHealthComponent()->OnHealthChanged.AddDynamic(this, &ABotAIController::HandleBotHealthChanged);
	}
}

void ABotAIController::HandleBotHealthChanged(float CurrentHealth, float MaxHealth)
{
	// Smart Reflex: If damaged from outside FOV, enable 360-degree threat awareness for 3 seconds
	if (CurrentHealth < PreviousHealth)
	{
		LastDamagedTime = GetWorld()->GetTimeSeconds();
	}
	PreviousHealth = CurrentHealth;
}

void ABotAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledBot || !ControlledBot->GetHealthComponent())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastDecisionTime >= 0.2f)
	{
		LastDecisionTime = CurrentTime;

		UpdatePerception();

		EvaluatePriorities();

		ExecuteCurrentState();
	}

	if (CurrentState == EBotState::EngagingTarget && CurrentTarget)
	{
		ExecuteCombat(DeltaTime);
	}
	else
	{
		FaceMovementDirection(DeltaTime);
	}
}

bool ABotAIController::IsPickupAvailable(AActor* PickupActor) const
{
	return PickupActor && !PickupActor->IsHidden() && PickupActor->GetActorEnableCollision();
}

bool ABotAIController::IsHealthPickupActor(AActor* Actor) const
{
	if (!Actor) return false;
	return Actor->ActorHasTag(TEXT("HealthPickup")) || Actor->GetClass()->GetName().Contains(TEXT("HealthPickup"));
}

bool ABotAIController::IsWeaponPickupActor(AActor* Actor) const
{
	if (!Actor) return false;
	return Actor->ActorHasTag(TEXT("WeaponPickup")) || Actor->GetClass()->GetName().Contains(TEXT("WeaponPickup"));
}

bool ABotAIController::CanSeeActor(AActor* TargetActor, bool bIgnoreFOV) const
{
	if (!ControlledBot || !TargetActor || TargetActor == ControlledBot || TargetActor->IsHidden())
	{
		return false;
	}

	const FVector BotEyes = ControlledBot->GetPawnViewLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();
	const FVector ToTarget = TargetLoc - BotEyes;

	if (ToTarget.SizeSquared() > FMath::Square(SightRadius))
	{
		return false;
	}

	if (!bIgnoreFOV)
	{
		const FVector Forward = ControlledBot->GetActorForwardVector();
		const float Dot = FVector::DotProduct(Forward, ToTarget.GetSafeNormal());
		const float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(VisionAngleDegrees * 0.5f));
		if (Dot < CosHalfFOV)
		{
			return false;
		}
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledBot);
	if (ControlledBot->GetCurrentWeapon())
	{
		Params.AddIgnoredActor(ControlledBot->GetCurrentWeapon());
	}

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, BotEyes, TargetLoc, ECC_Visibility, Params))
	{
		return (Hit.GetActor() == TargetActor);
	}

	return true;
}

void ABotAIController::UpdatePerception()
{
	CurrentTarget = nullptr;
	VisibleHealthPickup = nullptr;
	VisibleWeaponPickup = nullptr;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const bool bRecentlyShot = (CurrentTime - LastDamagedTime) < 3.0f;

	// 1. Scan for Closest Visible Target (Player or other Bots)
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllCharacters);

	float ClosestTargetDistSq = FLT_MAX;
	for (AActor* Actor : AllCharacters)
	{
		APlayerCharacter* Candidate = Cast<APlayerCharacter>(Actor);
		if (!Candidate || Candidate == ControlledBot)
		{
			continue;
		}

		if (Candidate->GetHealthComponent() && Candidate->GetHealthComponent()->GetCurrentHealth() <= 0.0f)
		{
			continue;
		}

		if (CanSeeActor(Candidate, bRecentlyShot))
		{
			const float DistSq = FVector::DistSquared(ControlledBot->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < ClosestTargetDistSq)
			{
				ClosestTargetDistSq = DistSq;
				CurrentTarget = Candidate;
			}
		}
	}

	// Update Target Memory (Object Permanence)
	if (CurrentTarget)
	{
		LastKnownTargetLocation = CurrentTarget->GetActorLocation();
		LastSeenTargetTime = CurrentTime;
		bHasLastKnownTarget = true;
	}
	else if (bHasLastKnownTarget && (CurrentTime - LastSeenTargetTime) > 4.0f)
	{
		bHasLastKnownTarget = false;
	}

	// 2. Scan for Pickups without coupling to concrete Pickup headers
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	float ClosestHealthDistSq = FLT_MAX;
	float ClosestAmmoDistSq = FLT_MAX;

	for (AActor* Actor : AllActors)
	{
		if (!IsPickupAvailable(Actor))
		{
			continue;
		}

		if (IsHealthPickupActor(Actor) && CanSeeActor(Actor, false))
		{
			const float DistSq = FVector::DistSquared(ControlledBot->GetActorLocation(), Actor->GetActorLocation());
			if (DistSq < ClosestHealthDistSq)
			{
				ClosestHealthDistSq = DistSq;
				VisibleHealthPickup = Actor;
			}
		}
		else if (IsWeaponPickupActor(Actor) && CanSeeActor(Actor, false))
		{
			const float DistSq = FVector::DistSquared(ControlledBot->GetActorLocation(), Actor->GetActorLocation());
			if (DistSq < ClosestAmmoDistSq)
			{
				ClosestAmmoDistSq = DistSq;
				VisibleWeaponPickup = Actor;
			}
		}
	}
}

void ABotAIController::EvaluatePriorities()
{
	const float CurrentHP =
		ControlledBot->GetHealthComponent()->GetCurrentHealth();

	// 1. Evade when health is below 25
	if (CurrentHP < 25.0f && CurrentTarget)
	{
		SetBotState(EBotState::EvadingTarget);
		return;
	}

	// 2. Engage visible targets
	if (CurrentTarget)
	{
		SetBotState(EBotState::EngagingTarget);
		return;
	}

	// 3. Seek health when below 75
	if (CurrentHP < 75.0f && VisibleHealthPickup)
	{
		SetBotState(EBotState::SeekingHealth);
		return;
	}

	// 4. Seek ammunition when needed
	if (ControlledBot->NeedsAmmo() && VisibleWeaponPickup)
	{
		SetBotState(EBotState::SeekingAmmo);
		return;
	}

	// 5. Investigate last known target position
	if (bHasLastKnownTarget && CurrentHP >= 50.0f)
	{
		SetBotState(EBotState::InvestigatingLastSeen);
		return;
	}

	// 6. Default behaviour
	SetBotState(EBotState::Patrolling);
}

void ABotAIController::SetBotState(EBotState NewState)
{
	static FPatrollingBotState PatrollingState;
	static FEngagingBotState EngagingState;
	static FEvadingBotState EvadingState;
	static FSeekingHealthBotState SeekingHealthState;
	static FSeekingAmmoBotState SeekingAmmoState;
	static FInvestigatingBotState InvestigatingState;

	CurrentState = NewState;

	switch (NewState)
	{
	case EBotState::Patrolling:
		ActiveState = &PatrollingState;
		break;

	case EBotState::EngagingTarget:
		ActiveState = &EngagingState;
		break;

	case EBotState::EvadingTarget:
		ActiveState = &EvadingState;
		break;

	case EBotState::SeekingHealth:
		ActiveState = &SeekingHealthState;
		break;

	case EBotState::SeekingAmmo:
		ActiveState = &SeekingAmmoState;
		break;

	case EBotState::InvestigatingLastSeen:
		ActiveState = &InvestigatingState;
		break;
	}
}

void ABotAIController::ExecuteCurrentState()
{
	if (ActiveState)
	{
		ActiveState->Execute(*this);
	}
}

void ABotAIController::ExecuteCombatMovement()
{
	if (!CurrentTarget)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastStrafeTime >= 1.1f)
	{
		LastStrafeTime = CurrentTime;

		if (UNavigationSystemV1* NavSys =
			UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			const FVector BotLoc = ControlledBot->GetActorLocation();

			const FVector ToTarget =
				CurrentTarget->GetActorLocation() - BotLoc;

			const float Dist = ToTarget.Size();

			const FVector RightVec =
				FVector::CrossProduct(
					ToTarget.GetSafeNormal2D(),
					FVector::UpVector
				);

			const float StrafeSign =
				FMath::RandBool() ? 1.0f : -1.0f;

			FVector ForwardAdjust = FVector::ZeroVector;

			if (Dist > 1400.0f)
			{
				ForwardAdjust = ToTarget.GetSafeNormal2D() * 250.0f;
			}
			else if (Dist < 500.0f)
			{
				ForwardAdjust = -ToTarget.GetSafeNormal2D() * 250.0f;
			}

			const FVector CombatMoveGoal =
				BotLoc + (RightVec * StrafeSign * 320.0f)
				+ ForwardAdjust;

			FNavLocation ProjectedLoc;

			if (NavSys->ProjectPointToNavigation(
				CombatMoveGoal, ProjectedLoc))
			{
				MoveToLocation(ProjectedLoc.Location, 15.0f);
			}
		}
	}
}

void ABotAIController::ExecuteCombat(float DeltaTime)
{
	if (!CurrentTarget)
	{
		return;
	}

	const FVector BotEyes = ControlledBot->GetPawnViewLocation();
	const FVector TargetLoc = CurrentTarget->GetActorLocation();
	const FRotator DesiredRot = (TargetLoc - BotEyes).Rotation();

	const FRotator NewRot = FMath::RInterpTo(GetControlRotation(), DesiredRot, DeltaTime, AimInterpSpeed);
	SetControlRotation(NewRot);
	ControlledBot->SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));

	const float AimAlignment = FVector::DotProduct(NewRot.Vector(), (TargetLoc - BotEyes).GetSafeNormal());
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (AimAlignment >= 0.96f && (CurrentTime - LastFireTime) >= FireInterval)
	{
		LastFireTime = CurrentTime;
		ControlledBot->FireWeapon();
	}
}

void ABotAIController::FaceMovementDirection(float DeltaTime)
{
	const FVector Velocity = ControlledBot->GetVelocity();
	if (Velocity.SizeSquared2D() > 25.0f)
	{
		const FRotator DesiredRot = Velocity.GetSafeNormal2D().Rotation();
		const FRotator NewRot = FMath::RInterpTo(GetControlRotation(), DesiredRot, DeltaTime, 8.0f);
		SetControlRotation(NewRot);
		ControlledBot->SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
	}
}

void ABotAIController::FleeToCoverOrSafety()
{
	if (!CurrentTarget)
	{
		return;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return;
	}

	const FVector BotLoc = ControlledBot->GetActorLocation();
	const FVector ThreatLoc = CurrentTarget->GetActorLocation();
	const FVector BaseAwayDir = (BotLoc - ThreatLoc).GetSafeNormal2D();

	FVector BestFleePoint = BotLoc;
	float BestScore = -FLT_MAX;

	// Evaluate 5 escape angles (-80, -40, 0, +40, +80 degrees away from threat)
	const float CandidateAngles[] = { 0.0f, -40.0f, 40.0f, -80.0f, 80.0f };
	for (float Angle : CandidateAngles)
	{
		const FVector RotatedDir = BaseAwayDir.RotateAngleAxis(Angle, FVector::UpVector);
		const FVector CandidateGoal = BotLoc + (RotatedDir * 950.0f);

		FNavLocation ProjectedLoc;
		if (NavSys->ProjectPointToNavigation(CandidateGoal, ProjectedLoc, FVector(300.0f, 300.0f, 200.0f)))
		{
			float Score = FVector::Dist2D(ProjectedLoc.Location, ThreatLoc);

			// Huge bonus if this escape point puts a wall/obstacle between the bot and the threat!
			FHitResult CoverHit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(ControlledBot);
			Params.AddIgnoredActor(CurrentTarget);
			if (GetWorld()->LineTraceSingleByChannel(
				CoverHit,
				ProjectedLoc.Location + FVector(0, 0, 50.0f),
				ThreatLoc + FVector(0, 0, 50.0f),
				ECC_Visibility,
				Params))
			{
				Score += 2000.0f; // Prioritizes genuine cover that breaks line-of-sight
			}

			if (Score > BestScore)
			{
				BestScore = Score;
				BestFleePoint = ProjectedLoc.Location;
			}
		}
	}

	MoveToLocation(BestFleePoint, 25.0f);
}

void ABotAIController::PatrolArena()
{
	if (GetMoveStatus() == EPathFollowingStatus::Moving)
	{
		return;
	}

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation RandomPt;
		if (NavSys->GetRandomReachablePointInRadius(ControlledBot->GetActorLocation(), 2200.0f, RandomPt))
		{
			MoveToLocation(RandomPt.Location, 30.0f);
		}
	}
}