#include "TLExecCalc_Damage.h"

#include "TLGameplayTags.h"
#include "AbilitySystem/Attribute/TLCombatSet.h"
#include "AbilitySystem/Attribute/TLHealthSet.h"

struct FECDamageStatics
{
	// Source의 공격력
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);

	FECDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTLCombatSet, AttackPower, Source, false);
	}
};

static const FECDamageStatics& DamageStatics()
{
	static FECDamageStatics Statics;
	return Statics;
}

UTLExecCalc_Damage::UTLExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
}

void UTLExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParams;

	// Source의 공격력을 가져옵니다.
	float AttackPower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParams,AttackPower);

	// 최종 데미지 = GA에서 전달한 데미지 배수 * Source의 AttackPower
	const float Damage = Spec.GetSetByCallerMagnitude(TLGameplayTags::SetByCaller_Damage, false, 0.0f);

	if (Damage <= 0.0f)
	{
		return;
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UTLHealthSet::GetDamageAttribute(),
			EGameplayModOp::Additive,
			Damage
		)
	);
}