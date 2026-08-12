// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Combat/TLCombatAbility_Melee.h"

#include "TLGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/Component/MeleeTraceComponent.h"

UTLCombatAbility_Melee::UTLCombatAbility_Melee()
{
}

void UTLCombatAbility_Melee::SetupAttackTasks()
{
	UAbilityTask_WaitGameplayEvent* HitEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TLGameplayTags::GameplayEvent_Attack_MeleeHit);

	HitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnAttackHit);
	HitEventTask->ReadyForActivation();
}

void UTLCombatAbility_Melee::OnAttackHit(FGameplayEventData EventData)
{
	if (EventData.TargetData.Num() == 0)
	{
		return;
	}

	const FHitResult* HitResult = EventData.TargetData.Get(0)->GetHitResult();
	if (!HitResult)
	{
		return;
	}

	HandleMeleeHit(*HitResult);
}

void UTLCombatAbility_Melee::OnMontageCancelled()
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (UMeleeTraceComponent* TraceComponent = AvatarActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			TraceComponent->EndTrace();
		}
	}
}

void UTLCombatAbility_Melee::HandleMeleeHit(const FHitResult& HitResult)
{
	AActor* TargetActor = HitResult.GetActor();	
	ApplyDamageToTarget(TargetActor, &HitResult);
}