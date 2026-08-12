#include "TLHealthSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/TLAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UTLHealthSet::UTLHealthSet() : Health(100.f), MaxHealth(100.f)
{
	bOutOfHealth = false;
}

void UTLHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float OldHealth = GetHealth();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float CurrentHealth = GetHealth();
		const float IncomingDamage = GetDamage();
		const float NewHealth = CurrentHealth - IncomingDamage;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Before SetHealth | Health: %.2f Damage: %.2f NewHealth: %.2f"),
			CurrentHealth,
			IncomingDamage,
			NewHealth);

		SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		SetDamage(0.0f);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("After SetHealth | Health: %.2f Damage: %.2f"),
			GetHealth(),
			GetDamage());
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(GetHealth() + GetHealing());
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	if (GetHealth() == OldHealth)
	{
		return;
	}

	const FGameplayEffectContextHandle& EffectContext =	Data.EffectSpec.GetEffectContext();

	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldHealth, GetHealth());

	if (GetHealth() <= 0.0f && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator,	Causer,	&Data.EffectSpec, Data.EvaluatedData.Magnitude,	OldHealth, GetHealth());
	}

	bOutOfHealth = GetHealth() <= 0.0f;
}

void UTLHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && GetHealth() > NewValue)
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && GetHealth() > 0.0f)
	{
		bOutOfHealth = false;
	}
}

void UTLHealthSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTLHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTLHealthSet, MaxHealth,	COND_None, REPNOTIFY_Always);
}

void UTLHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UTLHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTLHealthSet, Health, OldValue);
}

void UTLHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTLHealthSet, MaxHealth, OldValue);
}