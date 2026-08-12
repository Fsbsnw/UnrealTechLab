#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TLGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// Input이 Trigger 됐을 경우 (Pressed/Released)
	OnInputTriggered,
	
	// Input이 Held되어 있을 경우
	WhileInputActive,
	
	// Avatar가 생성됐을 경우 바로 할당 (패시브 스킬)
	OnSpawn,

	// GameplayEvent 또는 코드의 명시적인 요청으로 발동
	ByGameplayEvent
};

UCLASS()
class UNREALTECHLAB_API UTLGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
// ─────────────────────────────────────────────────────────────
// GameplayAbility Interface
// ─────────────────────────────────────────────────────────────
public:
	UTLGameplayAbility();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
// ─────────────────────────────────────────────────────────────
// Activation Policy 
// ─────────────────────────────────────────────────────────────
public:
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	
private:
	/** 어빌리티가 발동되기 위한 정책입니다. */
	UPROPERTY(EditDefaultsOnly)
	EAbilityActivationPolicy ActivationPolicy;
};
