#include "TLInputConfig.h"

UTLInputConfig::UTLInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* UTLInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTLInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	
	return nullptr;
}

const UInputAction* UTLInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTLInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	
	return nullptr;
}

const UInputAction* UTLInputConfig::FindUIInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTLInputAction& Action : UIInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	
	return nullptr;
}
