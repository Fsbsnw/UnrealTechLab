// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TLAnimNotifyState_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTECHLAB_API UTLAnimNotifyState_MeleeAttack : public UAnimNotifyState
{
	GENERATED_BODY()

	UTLAnimNotifyState_MeleeAttack();
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};