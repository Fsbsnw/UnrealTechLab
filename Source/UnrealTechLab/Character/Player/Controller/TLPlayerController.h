#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TLPlayerController.generated.h"

UCLASS()
class UNREALTECHLAB_API ATLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
// ─────────────────────────────────────────────────────────────
// PlayerController Interface
// ─────────────────────────────────────────────────────────────
public:
	ATLPlayerController();
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
