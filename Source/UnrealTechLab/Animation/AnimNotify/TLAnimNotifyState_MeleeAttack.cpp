// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/TLAnimNotifyState_MeleeAttack.h"

#include "Character/Player/Component/MeleeTraceComponent.h"

UTLAnimNotifyState_MeleeAttack::UTLAnimNotifyState_MeleeAttack()
{
	bIsNativeBranchingPoint = true;
}

void UTLAnimNotifyState_MeleeAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* TraceComponent =	OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			TraceComponent->BeginTrace();
		}
	}
}

void UTLAnimNotifyState_MeleeAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UMeleeTraceComponent* TraceComponent =	OwnerActor->FindComponentByClass<UMeleeTraceComponent>())
		{
			TraceComponent->EndTrace();
		}
	}
}