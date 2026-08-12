// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TLCombatAbility.h"
#include "TLCombatAbility_Melee.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTECHLAB_API UTLCombatAbility_Melee  : public UTLCombatAbility
{
	GENERATED_BODY()

public:
	UTLCombatAbility_Melee();

protected:
	/**
	 * 자식 클래스가 공격 방식별 AbilityTask를 등록합니다.
	 * 예: 근접 히트 이벤트 대기, 투사체 생성 이벤트 대기
	 */
	virtual void SetupAttackTasks();

	/** 히트된 타겟을 이벤트로 전달받을 때 실행될 함수입니다. */
	UFUNCTION()
	void OnAttackHit(FGameplayEventData EventData);
	
	/** 히트된 타겟에게 데미지를 적용합니다. */
	void HandleMeleeHit(const FHitResult& HitResult);

	virtual void OnMontageCancelled() override;
};