#include "Actor/TLThrowingKnifeProjectile.h"

#include "Components/SceneComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

ATLThrowingKnifeProjectile::ATLThrowingKnifeProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	if (ProjectileMovement)
	{
		ProjectileMovement->ProjectileGravityScale = 0.0f;
	}
}

void ATLThrowingKnifeProjectile::LaunchDelayedHoming(USceneComponent* TargetComponent, const FVector& InitialVelocity)
{
	if (!HasAuthority() || !ProjectileMovement || InitialVelocity.IsNearlyZero())
	{
		return;
	}

	PendingHomingTarget = TargetComponent;
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = TargetComponent;
	ProjectileMovement->Velocity = InitialVelocity;
	ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;
	SetActorRotation(InitialVelocity.Rotation());

	if (!TargetComponent)
	{
		return;
	}

	if (HomingDelay <= 0.0f)
	{
		BeginDelayedHoming();
		return;
	}

	GetWorldTimerManager().SetTimer(
		HomingDelayTimer,
		this,
		&ThisClass::BeginDelayedHoming,
		HomingDelay,
		false);
}

void ATLThrowingKnifeProjectile::BeginDelayedHoming()
{
	USceneComponent* TargetComponent = PendingHomingTarget.Get();
	if (!TargetComponent || !ProjectileMovement)
	{
		return;
	}

	const FVector TargetDirection =
		(TargetComponent->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
	if (TargetDirection.IsNearlyZero())
	{
		return;
	}

	ProjectileMovement->Velocity = TargetDirection * HomingLaunchSpeed;
	ProjectileMovement->HomingTargetComponent = TargetComponent;
	ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;
	ProjectileMovement->bIsHomingProjectile = true;
}
