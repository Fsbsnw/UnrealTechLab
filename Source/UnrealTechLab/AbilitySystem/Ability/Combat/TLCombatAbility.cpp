// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Combat/TLCombatAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TLGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UTLCombatAbility::UTLCombatAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UTLCombatAbility::ApplyDamageToTarget(AActor* TargetActor, const FHitResult* HitResult)
{
	if (!TargetActor)
	{
		return false;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		return false;
	}

	FGameplayEffectSpecHandle DamageSpecHandle = MakeDamageEffectSpec(HitResult);
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
	
	return true;
}

void UTLCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주에서 이벤트를 수신하기 위해 Task를 등록합니다.
	SetupAttackTasks();

	// 몽타주를 실행시킵니다.
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage, AttackMontageRate);

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UTLCombatAbility::SetupAttackTasks()
{
	// 자식 클래스에서 구현 필요
}

FGameplayEffectSpecHandle UTLCombatAbility::MakeDamageEffectSpec(const FHitResult* HitResult) const
{
	if (!DamageEffectClass)
	{
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

	// Combat GameplayCue 등에 사용할 공격 타입
	DamageSpecHandle.Data->AddDynamicAssetTag(AttackTypeTag);
	DamageSpecHandle.Data->SetSetByCallerMagnitude(TLGameplayTags::SetByCaller_Damage, Damage);

	if (HitResult)
	{
		DamageSpecHandle.Data->GetContext().AddHitResult(*HitResult);
	}

	return DamageSpecHandle;
}

void UTLCombatAbility::OnMontageFinished()
{	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UTLCombatAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}