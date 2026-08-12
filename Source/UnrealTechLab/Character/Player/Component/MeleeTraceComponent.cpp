#include "MeleeTraceComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "DrawDebugHelpers.h"
#include "TLGameplayTags.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UMeleeTraceComponent::UMeleeTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMeleeTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		OwnerMesh = OwnerCharacter->GetMesh();
	}
}

void UMeleeTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAttackTracing)
	{
		PerformTrace();
	}
}

void UMeleeTraceComponent::BeginTrace()
{
	if (!OwnerMesh)
	{
		return;
	}
	
	SetComponentTickEnabled(true);
	HitActors.Reset();
	PreviousTraceStart = OwnerMesh->GetSocketLocation(TraceStartSocket);
	PreviousTraceEnd = OwnerMesh->GetSocketLocation(TraceEndSocket);

	bAttackTracing = true;
}

void UMeleeTraceComponent::EndTrace()
{
	SetComponentTickEnabled(false);
	bAttackTracing = false;
	HitActors.Reset();
}

void UMeleeTraceComponent::PerformTrace()
{
	if (!OwnerMesh)
	{
		return;
	}
	
	const FVector CurrentTraceStart = OwnerMesh->GetSocketLocation(TraceStartSocket);
	const FVector CurrentTraceEnd = OwnerMesh->GetSocketLocation(TraceEndSocket);

	TArray<FHitResult> HitResults;
	TArray<FHitResult> EndHitResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	const bool bStartHit = GetWorld()->SweepMultiByProfile(
		HitResults,
		PreviousTraceStart,
		CurrentTraceStart,
		FQuat::Identity,
		FName(TEXT("Pawn")),
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams);

	const bool bEndHit = GetWorld()->SweepMultiByProfile(
		EndHitResults,
		PreviousTraceEnd,
		CurrentTraceEnd,
		FQuat::Identity,
		FName(TEXT("Pawn")),
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams);

	HitResults.Append(EndHitResults);
	
	DrawTraceDebug(PreviousTraceStart, CurrentTraceStart, !HitResults.IsEmpty());
	DrawTraceDebug(PreviousTraceEnd, CurrentTraceEnd, !EndHitResults.IsEmpty());

	// 히트된 플레이어가 있으면 해당 정보를 GA에 전달 후 데미지 처리
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor || !HitActor->IsA<APawn>() || HitActors.Contains(HitActor))
		{
			continue;
		}

		HitActors.Add(HitActor);

		FGameplayEventData EventData;
		EventData.EventTag = TLGameplayTags::GameplayEvent_Attack_MeleeHit;
		EventData.Instigator = GetOwner();
		EventData.Target = HitActor;
		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), TLGameplayTags::GameplayEvent_Attack_MeleeHit,EventData);
	}

	PreviousTraceStart = CurrentTraceStart;
	PreviousTraceEnd = CurrentTraceEnd;
}

void UMeleeTraceComponent::DrawTraceDebug(const FVector& Start, const FVector& End, bool bHit) const
{
	if (!bDrawDebug)
	{
		return;
	}

	const FVector Center = (Start + End) * 0.5f;
	const FVector Direction = End - Start;
	const float HalfHeight = Direction.Size() * 0.5f + TraceRadius;
	const FQuat Rotation = FRotationMatrix::MakeFromZ(Direction).ToQuat();

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		TraceRadius,
		Rotation,
		bHit ? FColor::Green : FColor::Red,
		false,
		DebugLifetime);
}