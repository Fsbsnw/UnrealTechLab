#include "Animation/AnimInstance/TLBowAnimInstance.h"

void UTLBowAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheOwningPlayerCharacter();
}

void UTLBowAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(OwningPlayerCharacter))
	{
		CacheOwningPlayerCharacter();
	}

	BowState = IsValid(OwningPlayerCharacter)
		? OwningPlayerCharacter->GetBowAnimState()
		: ETLBowAnimState::Idle;
}

void UTLBowAnimInstance::NativeUninitializeAnimation()
{
	OwningPlayerCharacter = nullptr;
	BowState = ETLBowAnimState::Idle;

	Super::NativeUninitializeAnimation();
}

void UTLBowAnimInstance::CacheOwningPlayerCharacter()
{
	OwningPlayerCharacter = nullptr;

	AActor* OwningActor = GetOwningActor();
	if (!OwningActor)
	{
		return;
	}

	// 활 메시가 캐릭터에 직접 달린 경우입니다.
	OwningPlayerCharacter = Cast<ATLPlayerCharacterBase>(OwningActor);
	if (OwningPlayerCharacter)
	{
		return;
	}

	// BP_Archer_Bow처럼 별도 활 액터인 경우 SetOwner(Character)를 통해 찾습니다.
	OwningPlayerCharacter = Cast<ATLPlayerCharacterBase>(OwningActor->GetOwner());
	if (OwningPlayerCharacter)
	{
		return;
	}

	// ChildActor/부착 구조에서 Owner가 아직 설정되지 않은 초기화 시점을 보완합니다.
	OwningPlayerCharacter = Cast<ATLPlayerCharacterBase>(OwningActor->GetAttachParentActor());
}
