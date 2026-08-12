// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TLCombatAbility.h"
#include "TLCombatAbility_Projectile.generated.h"

class ATLProjectileBase;
struct FGameplayAbilityTargetDataHandle;
/**
 * 
 */
UCLASS()
class UNREALTECHLAB_API UTLCombatAbility_Projectile : public UTLCombatAbility
{
	GENERATED_BODY()

public:
	UTLCombatAbility_Projectile();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	void OnSpawnProjectile(FGameplayEventData EventData);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ATLProjectileBase> ProjectileClass;

	/** 투사체가 생성될 캐릭터 메시 소켓 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	FName ProjectileSpawnSocketName = TEXT("ArrowSocket");

	/** 카메라 중앙에서 조준점을 찾을 최대 거리입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Aim", meta = (ClampMin = "0.0", Units = "cm"))
	float AimTraceDistance = 10000.0f;

private:
	bool bLoadCompleted = false;
	bool bReleaseRequested = false;
	bool bHasAimPoint = false;
	bool bSpawnRequested = false;
	bool bProjectileSpawned = false;

	/** 이번 Ability 활성화에서 서버가 실제 발사에 사용할 조준점입니다. */
	FVector_NetQuantize CachedAimPoint = FVector::ZeroVector;

	UFUNCTION()
	void OnLoadCompleted(FGameplayEventData Payload);
	void StartFireSection();

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	FHitResult CalculateLocalAimHit();
	void SubmitAimTargetData();
	void OnAimTargetDataReceived(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ApplicationTag);
	void TrySpawnProjectile();
	void SpawnProjectileToward(const FVector& AimPoint);

// ===============================================
// First Person Animation
// ===============================================
	UPROPERTY(EditDefaultsOnly, Category="Attack|Montage")
	TObjectPtr<UAnimMontage> FirstPersonAttackMontage;

	void PlayFirstPersonMontage();
	void JumpMontagesToSection(FName SectionName);
};
