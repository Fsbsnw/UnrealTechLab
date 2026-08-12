#pragma once

#include "CoreMinimal.h"
#include "Actor/TLProjectileBase.h"
#include "TLThrowingKnifeProjectile.generated.h"

class USceneComponent;

/**
 * 짧은 시간 동안만 목표를 약하게 추적하는 투척 나이프 투사체입니다.
 * 목표를 지나치거나 목표에서 멀어지기 시작하면 유도를 영구히 해제합니다.
 */
UCLASS()
class UNREALTECHLAB_API ATLThrowingKnifeProjectile : public ATLProjectileBase
{
	GENERATED_BODY()

public:
	ATLThrowingKnifeProjectile();

	/** 서버에서 호출하여 약한 유도를 시작합니다. */
	void LaunchDelayedHoming(USceneComponent* TargetComponent, const FVector& InitialVelocity);
	
protected:
	/** 약한 방향 보정에 사용할 가속도입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Homing", meta = (ClampMin = "0.0", Units = "cm/s^2"))
	float HomingAcceleration = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Homing", meta = (ClampMin = "0.0", Units = "s"))
	float HomingDelay = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Homing", meta = (ClampMin = "0.0", Units = "cm/s"))
	float HomingLaunchSpeed = 3000.0f;

private:
	void BeginDelayedHoming();

	TWeakObjectPtr<USceneComponent> PendingHomingTarget;
	FTimerHandle HomingDelayTimer;
};
