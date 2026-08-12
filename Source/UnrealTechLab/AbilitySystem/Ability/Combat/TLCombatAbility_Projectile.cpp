// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Combat/TLCombatAbility_Projectile.h"

#include "AbilitySystemComponent.h"
#include "TLGameplayTags.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Actor/TLProjectileBase.h"
#include "Character/Player/TLPlayerCharacterBase.h"
#include "GameplayPrediction.h"

UTLCombatAbility_Projectile::UTLCombatAbility_Projectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UTLCombatAbility_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bLoadCompleted = false;
	bReleaseRequested = false;
	bHasAimPoint = false;
	bSpawnRequested = false;
	bProjectileSpawned = false;
	CachedAimPoint = FVector::ZeroVector;

	if (!AttackMontage || !ProjectileClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 원격 소유 클라이언트가 보내는 이번 발사의 조준 TargetData를 서버에서 기다린다.
	if (ActorInfo->IsNetAuthority() && !ActorInfo->IsLocallyControlled())
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			const FPredictionKey ActivationPredictionKey = ActivationInfo.GetActivationPredictionKey();
			ASC->AbilityTargetDataSetDelegate(Handle, ActivationPredictionKey)
				.AddUObject(this, &ThisClass::OnAimTargetDataReceived);
			ASC->CallReplicatedTargetDataDelegatesIfSet(Handle, ActivationPredictionKey);
		}
	}

	// 장전 완료 이벤트 대기
	UAbilityTask_WaitGameplayEvent* LoadCompletedTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TLGameplayTags::GameplayEvent_Attack_Nocked);

	LoadCompletedTask->EventReceived.AddDynamic(this, &ThisClass::OnLoadCompleted);
	LoadCompletedTask->ReadyForActivation();

	// 발사
	UAbilityTask_WaitGameplayEvent* SpawnEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TLGameplayTags::GameplayEvent_Attack_SpawnProjectile);
	
	SpawnEventTask ->EventReceived.AddDynamic(this, &ThisClass::OnSpawnProjectile);
	SpawnEventTask ->ReadyForActivation();

	// 발사 입력
	UAbilityTask_WaitInputRelease* ReleaseTask =
		UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);

	ReleaseTask->OnRelease.AddDynamic(this,	&ThisClass::OnInputReleased);
	ReleaseTask->ReadyForActivation();

	if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		Character->SetBowAnimState(ETLBowAnimState::Drawing);
	}

	PlayFirstPersonMontage();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UTLCombatAbility_Projectile::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->IsNetAuthority())
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey()).RemoveAll(this);
		}
	}

	if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		Character->SetBowAnimState(ETLBowAnimState::Idle);
	}

	bLoadCompleted = false;
	bReleaseRequested = false;
	bHasAimPoint = false;
	bSpawnRequested = false;
	bProjectileSpawned = false;
	CachedAimPoint = FVector::ZeroVector;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTLCombatAbility_Projectile::OnSpawnProjectile(FGameplayEventData EventData)
{
	if (const ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		// 발사 Notify 순간 소유 플레이어가 실제로 보고 있는 화면을 조준 기준으로 확정한다.
		if (Character->IsLocallyControlled())
		{
			SubmitAimTargetData();
		}
	}

	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	bSpawnRequested = true;
	TrySpawnProjectile();
}

void UTLCombatAbility_Projectile::TrySpawnProjectile()
{
	if (!HasAuthority(&CurrentActivationInfo) || !bHasAimPoint || !bSpawnRequested || bProjectileSpawned)
	{
		return;
	}

	bProjectileSpawned = true;
	SpawnProjectileToward(CachedAimPoint);
}

void UTLCombatAbility_Projectile::SpawnProjectileToward(const FVector& AimPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Projectile Actor"));

	ATLPlayerCharacterBase* Character =	Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character || !DamageEffectClass || !ProjectileClass)
	{
		return;
	}

	USkeletalMeshComponent* ThirdPersonMesh = Character->GetMesh();
	if (!ThirdPersonMesh || !ThirdPersonMesh->DoesSocketExist(ProjectileSpawnSocketName))
	{
		return;
	}

	const FVector SpawnLocation = ThirdPersonMesh->GetSocketLocation(ProjectileSpawnSocketName);
	const FVector FireDirection = (AimPoint - SpawnLocation).GetSafeNormal();
	if (FireDirection.IsNearlyZero())
	{
		return;
	}

	const FRotator SpawnRotation = FireDirection.Rotation();

	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ATLProjectileBase* Projectile =
		GetWorld()->SpawnActorDeferred<ATLProjectileBase>(
			ProjectileClass,
			SpawnTransform,
			Character,
			Character,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!Projectile)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectSpecHandle DamageSpecHandle = MakeDamageEffectSpec();

	Projectile->InitializeProjectile(SourceASC, DamageSpecHandle);	
	Projectile->FinishSpawning(SpawnTransform);
}

void UTLCombatAbility_Projectile::OnLoadCompleted(FGameplayEventData Payload)
{
	bLoadCompleted = true;

	if (bReleaseRequested)
	{
		StartFireSection();
	}
	else
	{
		if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
		{
			Character->SetBowAnimState(ETLBowAnimState::Holding);
		}

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (!ASC)
		{
			return;
		}

		JumpMontagesToSection(TEXT("Hold"));
	}
}

void UTLCombatAbility_Projectile::StartFireSection()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	if (ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		Character->SetBowAnimState(ETLBowAnimState::Firing);
	}

	JumpMontagesToSection(TEXT("Fire"));
}

void UTLCombatAbility_Projectile::OnInputReleased(float TimeHeld)
{
	bReleaseRequested = true;

	if (bLoadCompleted)
	{
		StartFireSection();
	}
}

FHitResult UTLCombatAbility_Projectile::CalculateLocalAimHit()
{
	FHitResult AimHit;
	const ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return AimHit;
	}

	FVector ViewLocation = Character->GetPawnViewLocation();
	FRotator ViewRotation = Character->GetViewRotation();

	if (AController* Controller = Character->GetController())
	{
		Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}

	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * AimTraceDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BowAimTrace), false, Character);
	QueryParams.AddIgnoredActor(Character);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		AimHit,
		ViewLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams);

	AimHit.TraceStart = ViewLocation;
	AimHit.TraceEnd = TraceEnd;

	if (!bHit)
	{
		AimHit.Location = TraceEnd;
		AimHit.ImpactPoint = TraceEnd;
	}

	return AimHit;
}

void UTLCombatAbility_Projectile::SubmitAimTargetData()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	const FHitResult AimHit = CalculateLocalAimHit();
	const FVector AimPoint = AimHit.bBlockingHit ? AimHit.ImpactPoint : AimHit.Location;
	if (AimPoint.ContainsNaN())
	{
		return;
	}

	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(AimHit));

	if (HasAuthority(&CurrentActivationInfo))
	{
		// Listen Server의 로컬 플레이어는 RPC 없이 같은 검증 경로를 사용한다.
		OnAimTargetDataReceived(TargetData, FGameplayTag());
		return;
	}

	FScopedPredictionWindow ScopedPrediction(ASC, true);
	ASC->CallServerSetReplicatedTargetData(
		CurrentSpecHandle,
		CurrentActivationInfo.GetActivationPredictionKey(),
		TargetData,
		FGameplayTag(),
		ASC->ScopedPredictionKey);
}

void UTLCombatAbility_Projectile::OnAimTargetDataReceived(
	const FGameplayAbilityTargetDataHandle& TargetData,
	FGameplayTag ApplicationTag)
{
	if (!HasAuthority(&CurrentActivationInfo) || TargetData.Num() == 0)
	{
		return;
	}

	const FGameplayAbilityTargetData* AimData = TargetData.Get(0);
	const FHitResult* AimHit = AimData ? AimData->GetHitResult() : nullptr;

	if (AimHit)
	{
		const FVector ClientAimPoint = AimHit->bBlockingHit ? AimHit->ImpactPoint : AimHit->Location;
		if (!ClientAimPoint.ContainsNaN())
		{
			if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
			{
				const FVector Origin = AvatarActor->GetActorLocation();
				CachedAimPoint = Origin + (ClientAimPoint - Origin).GetClampedToMaxSize(AimTraceDistance);
				bHasAimPoint = true;
			}
		}
	}

	if (CurrentActorInfo && !CurrentActorInfo->IsLocallyControlled())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->ConsumeClientReplicatedTargetData(
				CurrentSpecHandle,
				CurrentActivationInfo.GetActivationPredictionKey());
		}
	}

	TrySpawnProjectile();
}

void UTLCombatAbility_Projectile::PlayFirstPersonMontage()
{
	ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo());

	// 소유 클라이언트만 1P 몽타주 실행
	if (!Character || !Character->IsLocallyControlled() || !FirstPersonAttackMontage)
	{
		return;
	}

	USkeletalMeshComponent* FirstPersonMesh = Character->GetFirstPersonMesh();

	if (!FirstPersonMesh)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(FirstPersonAttackMontage,	AttackMontageRate);
	}
}

void UTLCombatAbility_Projectile::JumpMontagesToSection(FName SectionName)
{
	// 3P 기준 몽타주
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->CurrentMontageJumpToSection(SectionName);
	}

	// 1P 소유자 몽타주
	ATLPlayerCharacterBase* Character = Cast<ATLPlayerCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character || !Character->IsLocallyControlled())
	{
		return;
	}

	USkeletalMeshComponent* FirstPersonMesh = Character->GetFirstPersonMesh();

	if (!FirstPersonMesh)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(SectionName,FirstPersonAttackMontage);
	}
}
