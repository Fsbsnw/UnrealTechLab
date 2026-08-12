#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TLAttributeSet.h"
#include "TLCombatSet.generated.h"

UCLASS()
class UNREALTECHLAB_API UTLCombatSet : public UTLAttributeSet
{
	GENERATED_BODY()

	friend struct FECDamageStatics;
// ─────────────────────────────────────────────────────────────
// AttributeSet Interface
// ─────────────────────────────────────────────────────────────
public:
	ATTRIBUTE_ACCESSORS(UTLCombatSet, AttackPower);


// ─────────────────────────────────────────────────────────────
// Clamp Method
// ─────────────────────────────────────────────────────────────
protected:
	/** Attribute 값을 보정하는 함수입니다. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;


// ─────────────────────────────────────────────────────────────
// Attribute Variable
// ─────────────────────────────────────────────────────────────
private:
	/** 기본 공격력입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;
};