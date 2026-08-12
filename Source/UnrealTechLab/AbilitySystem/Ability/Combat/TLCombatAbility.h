// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/TLGameplayAbility.h"
#include "TLCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTECHLAB_API UTLCombatAbility  : public UTLGameplayAbility
{
	GENERATED_BODY()

public:
	UTLCombatAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 자식 클래스가 공격 방식별 AbilityTask를 등록합니다.
	 * 예: 근접 히트 이벤트 대기, 투사체 생성 이벤트 대기
	 */
	virtual void SetupAttackTasks();

	FGameplayEffectSpecHandle MakeDamageEffectSpec(const FHitResult* HitResult = nullptr) const;
	
	/** 타겟에게 데미지 GameplayEffect를 적용합니다. */
	bool ApplyDamageToTarget(AActor* TargetActor, const FHitResult* HitResult = nullptr);
	
	// /** GameplayCue에 전달할 무기 타입을 정의합니다. */
	// UPROPERTY(EditDefaultsOnly)
	// FGameplayTag WeaponTypeTag = TLGameplayTags::;

	/** GameplayCue에 전달할 공격 타입을 정의합니다. */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackTypeTag;

	/** 공격에 실행할 애니메이션 몽타주입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | Montage")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 애니메이션 실행 속도입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack | Montage")
	float AttackMontageRate = 1.f;

	UFUNCTION()
	virtual void OnMontageFinished();

	UFUNCTION()
	virtual void OnMontageCancelled();
	
// ─────────────────────────────────────────────────────────────
// 데미지 정보
// ─────────────────────────────────────────────────────────────
	
protected:
	/** 적용할 데미지 (GE_Damage) 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** 기본 체력 데미지 수치입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 1.f;
};