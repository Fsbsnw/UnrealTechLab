#include "TLAbilitySet.h"

#include "TLAbilitySystemComponent.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(UTLAbilitySystemComponent* ASC)
{
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
}

void UTLAbilitySet::GiveToAbilitySystem(UTLAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject)
{
	check(ASC);

	// GiveAbility는 서버에서만 가능
	if (!ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GiveToAbilitySystem skipped on client. Owner: %s"),
			*GetNameSafe(ASC->GetOwnerActor()));

		return;
	}

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		
		FGameplayAbilitySpec AbilitySpec(AbilityToGrant.Ability, AbilityToGrant.AbilityLevel);
		// 어빌리티를 발동한 대상과 태그를 등록합니다.
		// 대상 정보는 디버깅에 활용하고, 태그는 ASC에서 InputTag에 해당하는 Ability를 탐색할 때 활용합니다.
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);
		
		// 어빌리티를 부여하고, OutGrantedHandles가 유효하면 어빌리티 정보를 핸들에 기록합니다.
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}
