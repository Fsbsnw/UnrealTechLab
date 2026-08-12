#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Combat/TLCombatAbility.h"
#include "TLCombatAbility_ThrowingKnife.generated.h"

class ATLThrowingKnifeProjectile;

/**
 * Minimal throwing ability: finds one target in front of the camera and spawns
 * a configurable number of projectiles in a centered horizontal row.
 */
UCLASS()
class UNREALTECHLAB_API UTLCombatAbility_ThrowingKnife : public UTLCombatAbility
{
	GENERATED_BODY()

public:
	UTLCombatAbility_ThrowingKnife();

protected:
	virtual void SetupAttackTasks() override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

// =================================================
// 투사체 정보
// =================================================
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing")
	TSubclassOf<ATLThrowingKnifeProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing", meta = (ClampMin = "1", ClampMax = "32"))
	int32 ProjectileCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing", meta = (ClampMin = "0.0", Units = "cm"))
	float ProjectileSpacing = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing", meta = (Units = "cm"))
	float SpawnForwardOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Homing")
	bool bEnableHoming = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Homing", meta = (ClampMin = "0.0", Units = "cm"))
	float TargetSearchDistance = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Homing", meta = (ClampMin = "0.0", Units = "cm"))
	float TargetSearchRadius = 500.0f;

	/** Initial outward bias of the outer projectiles before homing curves them back toward the target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Homing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float OutwardArcStrength = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Homing")
	TEnumAsByte<ECollisionChannel> TargetObjectChannel = ECC_Pawn;

	UPROPERTY(EditDefaultsOnly)
	float OutwardSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float UpwardSpeed = 500.f;
	
private:
	UFUNCTION()
	void OnSpawnProjectiles(FGameplayEventData EventData);

	AActor* FindHomingTarget(const FVector& ViewLocation, const FVector& ViewRotation) const;
	void SpawnProjectiles();

// =================================================
// 플레이어 이동
// =================================================
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Movement")
	float InitialBackwardSpeed = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Movement")
	float InitialUpwardSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Movement")
	float InitialMovementDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Movement")
	float HoverBackwardSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing|Movement")
	float HoverDuration = 0.66f;
	
private:
	void StartAerialMovement();

	UFUNCTION()
	void BeginHover();

	UFUNCTION()
	void BeginFalling();

	void RestoreFallingMovement();

	TWeakObjectPtr<ACharacter> AerialCharacter;
	FVector CachedBackwardDirection = FVector::ZeroVector;
	bool bControllingAerialMovement = false;
};
