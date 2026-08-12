#include "TLPlayerState.h"

#include "AbilitySystem/TLAbilitySet.h"
#include "AbilitySystem/Attribute/TLHealthSet.h"

ATLPlayerState::ATLPlayerState()
{
	{
		ASC = CreateDefaultSubobject<UTLAbilitySystemComponent>(TEXT("ASC"));
		ASC->SetIsReplicated(true);
		ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	}

	CreateDefaultSubobject<UTLHealthSet>(TEXT("HealthSet"));
	SetNetUpdateFrequency(100.0f);
}

void ATLPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InitAbilities();
	}
}

void ATLPlayerState::InitAbilities()
{
	for (UTLAbilitySet* AbilitySet : DefaultAbilitySets)
	{
		if (AbilitySet)
		{
			// OutGrantedHandle을 nullptr로 지정한다는 것은 영구 부여한다는 의미입니다.
			AbilitySet->GiveToAbilitySystem(ASC, nullptr);
		}
	}
}