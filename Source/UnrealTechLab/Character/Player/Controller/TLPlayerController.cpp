#include "TLPlayerController.h"

#include "AbilitySystem/TLAbilitySystemComponent.h"
#include "Character/Player/State/TLPlayerState.h"

ATLPlayerController::ATLPlayerController()
{
}

void ATLPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (const ATLPlayerState* PS = GetPlayerState<ATLPlayerState>())
	{
		if (UTLAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			ASC->ProcessAbilityInput(DeltaTime,bGamePaused);
		}	
	}
	Super::PostProcessInput(DeltaTime, bGamePaused);
}