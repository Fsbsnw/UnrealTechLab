#include "AbilitySystem/Ability/Combat/TLCombatAbility_ThrowingKnife.h"

#include "TLGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actor/TLThrowingKnifeProjectile.h"
#include "Character/Player/TLPlayerCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UTLCombatAbility_ThrowingKnife::UTLCombatAbility_ThrowingKnife()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UTLCombatAbility_ThrowingKnife::SetupAttackTasks()
{
	UAbilityTask_WaitGameplayEvent* SpawnEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			TLGameplayTags::GameplayEvent_Attack_SpawnProjectile);

	SpawnEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpawnProjectiles);
	SpawnEventTask->ReadyForActivation();
}

void UTLCombatAbility_ThrowingKnife::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsActive() || !ActorInfo)
	{
		return;
	}

	// 소유 클라이언트 예측 + 서버 권한 실행
	if (ActorInfo->IsLocallyControlled() ||	ActorInfo->IsNetAuthority())
	{
		if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
		{
			Character->SetMeshPerspective(false);
		}
		
		StartAerialMovement();
	}
}

void UTLCombatAbility_ThrowingKnife::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	RestoreFallingMovement();
	AerialCharacter.Reset();

	if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		Character->SetMeshPerspective(true);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTLCombatAbility_ThrowingKnife::OnSpawnProjectiles(FGameplayEventData EventData)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnProjectiles();
	}
}

void UTLCombatAbility_ThrowingKnife::SpawnProjectiles()
{
	UWorld* World = GetWorld();
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!World || !AvatarPawn || !ProjectileClass)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(AvatarPawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector CameraForward = CameraRotation.Vector();
	const FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	AActor* HomingTarget = FindHomingTarget(CameraLocation, CameraForward);

	const int32 Count = FMath::Max(ProjectileCount, 1);
	const float CenterIndex = static_cast<float>(Count - 1) * 0.5f;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const float SpawnRightOffset =
			(static_cast<float>(Index) - CenterIndex) * ProjectileSpacing;
		const FVector SpawnLocation =
			CameraLocation
			+ CameraForward * SpawnForwardOffset
			+ CameraRight * SpawnRightOffset;

		FTransform SpawnTransform(CameraRotation, SpawnLocation);
		ATLThrowingKnifeProjectile* Knife =
			World->SpawnActorDeferred<ATLThrowingKnifeProjectile>(
				ProjectileClass,
				SpawnTransform,
				AvatarPawn,
				AvatarPawn,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (!Knife)
		{
			continue;
		}

		const float SideRatio = CenterIndex > 0.0f
			? (static_cast<float>(Index) - CenterIndex) / CenterIndex
			: 0.0f;
		const FVector OutwardVelocity = CameraRight * SideRatio * OutwardSpeed;
		const FVector UpwardVelocity = FVector::UpVector * UpwardSpeed;

		UProjectileMovementComponent* Movement = Knife->GetProjectileMovementComponent();
		const float InitialSpeed = Movement ? Movement->InitialSpeed : 3000.0f;
		const FVector InitialVelocity =
			(CameraForward * InitialSpeed + OutwardVelocity + UpwardVelocity)
			.GetSafeNormal() * InitialSpeed;

		SpawnTransform.SetRotation(InitialVelocity.Rotation().Quaternion());

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		FGameplayEffectSpecHandle DamageSpecHandle = MakeDamageEffectSpec();

		Knife->InitializeProjectile(SourceASC, DamageSpecHandle);
		
		UGameplayStatics::FinishSpawningActor(Knife, SpawnTransform);

		Knife->LaunchDelayedHoming(HomingTarget ? HomingTarget->GetRootComponent() : nullptr, InitialVelocity);
	}
}

void UTLCombatAbility_ThrowingKnife::StartAerialMovement()
{
	ACharacter* Character =	Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement =	Character->GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	FVector Forward = Character->GetBaseAimRotation().Vector();

	Forward.Z = 0.0f;
	Forward.Normalize();

	if (Forward.IsNearlyZero())
	{
		Forward = Character->GetActorForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();
	}

	CachedBackwardDirection = -Forward;
	AerialCharacter = Character;
	bControllingAerialMovement = true;

	// 중력을 받지 않는 상태에서 뒤쪽 위로 이동
	Movement->SetMovementMode(MOVE_Flying);
	Movement->Velocity =
		CachedBackwardDirection * InitialBackwardSpeed
		+ FVector::UpVector * InitialUpwardSpeed;

	UAbilityTask_WaitDelay* InitialMovementTask =
		UAbilityTask_WaitDelay::WaitDelay(this, InitialMovementDuration);
	
	InitialMovementTask->OnFinish.AddDynamic(this, &ThisClass::BeginHover);
	InitialMovementTask->ReadyForActivation();
}

void UTLCombatAbility_ThrowingKnife::BeginHover()
{
	ACharacter* Character = AerialCharacter.Get();

	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement =	Character->GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	// Flying을 유지하며 Z 속도를 0으로 초기화한다.
	Movement->SetMovementMode(MOVE_Flying);
	Movement->Velocity = CachedBackwardDirection * HoverBackwardSpeed;

	UAbilityTask_WaitDelay* HoverTask =	UAbilityTask_WaitDelay::WaitDelay(this, HoverDuration);

	HoverTask->OnFinish.AddDynamic(this, &ThisClass::BeginFalling);
	HoverTask->ReadyForActivation();
}

void UTLCombatAbility_ThrowingKnife::BeginFalling()
{
	ACharacter* Character = AerialCharacter.Get();

	if (!Character)
	{
		return;
	}

	if (UCharacterMovementComponent* Movement =
		Character->GetCharacterMovement())
	{
		// 현재 Z 속도 0에서 중력이 다시 적용된다.
		Movement->SetMovementMode(MOVE_Falling);
	}

	bControllingAerialMovement = false;
}

void UTLCombatAbility_ThrowingKnife::RestoreFallingMovement()
{
	if (!bControllingAerialMovement)
	{
		return;
	}

	if (ACharacter* Character = AerialCharacter.Get())
	{
		if (UCharacterMovementComponent* Movement =	Character->GetCharacterMovement())
		{
			Movement->SetMovementMode(MOVE_Falling);
		}
	}

	bControllingAerialMovement = false;
}

AActor* UTLCombatAbility_ThrowingKnife::FindHomingTarget(const FVector& ViewLocation, const FVector& ViewDirection) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector TraceEnd = ViewLocation + ViewDirection * TargetSearchDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(TargetObjectChannel);

	FHitResult HitResult;
	const bool bHit = World->SweepSingleByObjectType(
		HitResult,
		ViewLocation,
		TraceEnd,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(TargetSearchRadius),
		QueryParams);

	return bHit ? HitResult.GetActor() : nullptr;
}
