#include "TLAttributeSet.h"
#include "AbilitySystem/TLAbilitySystemComponent.h"

void UTLAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UTLAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

UTLAbilitySystemComponent* UTLAttributeSet::GetAbilitySystemComponent() const
{
	return Cast<UTLAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}