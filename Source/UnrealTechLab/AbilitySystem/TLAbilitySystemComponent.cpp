#include "TLAbilitySystemComponent.h"

#include "Ability/TLGameplayAbility.h"

UTLAbilitySystemComponent::UTLAbilitySystemComponent()
{
}

void UTLAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (!Spec.IsActive())
	{
		return;
	}

	const UGameplayAbility* AbilityInstance = Spec.GetPrimaryInstance();

	const FPredictionKey PredictionKey =
		AbilityInstance ?
			AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();

	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,PredictionKey);
}

void UTLAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
	}
}

void UTLAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void UTLAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (bGamePaused)
	{
		InputPressedSpecHandles.Reset();
		InputReleasedSpecHandles.Reset();
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// 누르고 있는 동안 발동하는 Ability 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (!AbilitySpec->IsActive())
			{
				const UTLGameplayAbility* Ability =	CastChecked<UTLGameplayAbility>(AbilitySpec->Ability);

				if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 이번 프레임에 눌린 입력 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			AbilitySpec->InputPressed = true;

			if (AbilitySpec->IsActive())
			{
				// 이미 실행 중인 Ability에 입력 눌림 전달
				AbilitySpecInputPressed(*AbilitySpec);
			}
			else
			{
				const UTLGameplayAbility* Ability =	CastChecked<UTLGameplayAbility>(AbilitySpec->Ability);

				if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 수집한 Ability 발동
	for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	// 이번 프레임에 뗀 입력 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			AbilitySpec->InputPressed = false;

			if (AbilitySpec->IsActive())
			{
				AbilitySpecInputReleased(*AbilitySpec);
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}