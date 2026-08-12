#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeTraceComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMeleeTraceComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMeleeTraceComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime,	ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 근접 공격을 활성화합니다. */
	void BeginTrace();

	/** 근접 공격을 종료합니다. */
	void EndTrace();

private:
	/** 근접 공격을 틱마다 추적합니다. */
	void PerformTrace();

private:
	/** 공격 판정 구간의 시작 소켓입니다. */
	UPROPERTY(EditDefaultsOnly, Category="Trace", meta=(AllowPrivateAccess="true"))
	FName TraceStartSocket = TEXT("TraceStart");

	/** 공격 판정 구간의 끝 소켓입니다. */
	UPROPERTY(EditDefaultsOnly, Category="Trace", meta=(AllowPrivateAccess="true"))
	FName TraceEndSocket = TEXT("TraceEnd");

	/** 각 지점에서 수행할 Sphere Sweep 반경입니다. */
	UPROPERTY(EditDefaultsOnly, Category="Trace", meta=(ClampMin="0.0", AllowPrivateAccess="true"))
	float TraceRadius = 20.0f;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> OwnerMesh;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> HitActors;

	FVector PreviousTraceStart = FVector::ZeroVector;
	FVector PreviousTraceEnd = FVector::ZeroVector;

	bool bAttackTracing = false;
// ─────────────────────────────────────────────────────────────
// Debug
// ─────────────────────────────────────────────────────────────
private:
	void DrawTraceDebug(const FVector& Start, const FVector& End, bool bHit) const;
	
	UPROPERTY(EditDefaultsOnly, Category="Debug", meta=(AllowPrivateAccess="true"))
	bool bDrawDebug = false;

	UPROPERTY(EditDefaultsOnly, Category="Debug", meta=(ClampMin="0.0", AllowPrivateAccess="true"))
	float DebugLifetime = 0.05f;
};