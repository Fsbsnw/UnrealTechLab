#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/TLAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "TLPlayerState.generated.h"

class UTLAbilitySet;

UCLASS()
class UNREALTECHLAB_API ATLPlayerState : public APlayerState
{
	GENERATED_BODY()
	
// ─────────────────────────────────────────────────────────────
// PlayerState Interface
// ─────────────────────────────────────────────────────────────	
public:
	ATLPlayerState();
	virtual void BeginPlay() override;
	

// ─────────────────────────────────────────────────────────────
// Ability Method
// ─────────────────────────────────────────────────────────────
private:
	/** 플레이어가 기본으로 사용하는 어빌리티를 등록하는 함수입니다. */
	void InitAbilities();

private:
	/** 기본으로 사용하는 어빌리티에 대한 정보입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Default Abilities")
	TArray<TObjectPtr<UTLAbilitySet>> DefaultAbilitySets;
	
// ─────────────────────────────────────────────────────────────
// Component
// ─────────────────────────────────────────────────────────────		
protected:
	/** 플레이어의 AbilitySystemComponent입니다. OnPossess 호출 시점에 동기화됩니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTLAbilitySystemComponent> ASC;


// ─────────────────────────────────────────────────────────────
// Getter
// ─────────────────────────────────────────────────────────────
public:
	FORCEINLINE UTLAbilitySystemComponent* GetAbilitySystemComponent() const { return ASC; }
};
