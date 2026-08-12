#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/Player/TLPlayerCharacterBase.h"
#include "TLBowAnimInstance.generated.h"

/**
 * 활 SkeletalMesh용 AnimInstance입니다.
 * 활 액터의 Owner(플레이어 캐릭터)가 가진 복제 BowAnimState를 AnimGraph에 전달합니다.
 */
UCLASS(Blueprintable, BlueprintType, Transient)
class UNREALTECHLAB_API UTLBowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUninitializeAnimation() override;

	UFUNCTION(BlueprintPure, Category = "Bow")
	ETLBowAnimState GetBowState() const { return BowState; }

	UFUNCTION(BlueprintPure, Category = "Bow")
	ATLPlayerCharacterBase* GetOwningPlayerCharacter() const { return OwningPlayerCharacter; }

protected:
	/** AnimGraph 상태 전환에서 읽는 현재 활 상태입니다. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Bow")
	ETLBowAnimState BowState = ETLBowAnimState::Idle;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Bow")
	TObjectPtr<ATLPlayerCharacterBase> OwningPlayerCharacter;

private:
	void CacheOwningPlayerCharacter();
};
