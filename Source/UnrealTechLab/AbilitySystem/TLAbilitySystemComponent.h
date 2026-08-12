#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TLAbilitySystemComponent.generated.h"

class UTLGameplayAbility;

UCLASS()
class UNREALTECHLAB_API UTLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
// ─────────────────────────────────────────────────────────────
// AbilitySystemComponent Interface
// ─────────────────────────────────────────────────────────────
public:
	UTLAbilitySystemComponent();

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	
protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};