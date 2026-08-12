#include "TLGameplayAbility.h"

#include "AbilitySystem/TLAbilitySystemComponent.h"

UTLGameplayAbility::UTLGameplayAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTLGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 부여되는 즉시, OnSpawn 정책이라면 자동 발동을 시도합니다.
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UTLGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// 아바타(캐릭터)가 안정적인 상태일 때만 발동합니다.
		if (ASC && AvatarActor && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			ASC->TryActivateAbility(Spec.Handle);
		}
	}
}