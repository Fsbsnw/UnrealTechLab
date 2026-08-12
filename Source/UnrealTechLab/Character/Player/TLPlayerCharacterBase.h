// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Character/Common/TLCharacterBase.h"
#include "TLPlayerCharacterBase.generated.h"

class UCameraComponent;
class UTLInputConfig;
struct FInputActionValue;
class FLifetimeProperty;

UENUM(BlueprintType)
enum class ETLBowAnimState : uint8
{
	Idle,
	Drawing,
	Holding,
	Firing
};

UCLASS()
class UNREALTECHLAB_API ATLPlayerCharacterBase : public ATLCharacterBase
{
	GENERATED_BODY()
	
public:	
	ATLPlayerCharacterBase();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Bow")
	void SetBowAnimState(ETLBowAnimState NewState);

	UFUNCTION(BlueprintPure, Category = "Bow")
	ETLBowAnimState GetBowAnimState() const { return BowAnimState; }

	void SetMeshPerspective(bool bFirstPerson);

protected:
	void InitAbilityActorInfo();

protected:
	/** 플레이어에게 부여할 입력 액션입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TObjectPtr<UTLInputConfig> InputConfig;
	
	/** 플레이어에게 부여할 InputMappingContext입니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

private:
	/** 플레이어 이동 입력에 따라 캐릭터를 움직이기 위한 바인딩하는 함수입니다. */
	void Input_Move(const FInputActionValue& InputActionValue);
	
	/** 플레이어의 마우스 움직임에 따라 카메라를 회전하기 위한 바인딩 함수입니다. */
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	/** InputTag 입력에 따라 Ability를 발동시키기 위한 바인딩 함수입니다. */
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	
	/** InputTag 입력 종료에 따라 Ability를 중단하기 위한 바인딩 함수입니다. */
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FPMesh;

	/** GA가 변경하고 FP/TP 활 AnimBP가 공통으로 읽는 현재 활 동작 상태입니다. */
	UPROPERTY(ReplicatedUsing = OnRep_BowAnimState, BlueprintReadOnly, Category = "Bow")
	ETLBowAnimState BowAnimState = ETLBowAnimState::Idle;

	UFUNCTION()
	void OnRep_BowAnimState();

public:
	USkeletalMeshComponent* GetFirstPersonMesh();

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector FirstPersonCameraLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector ThirdPersonCameraLocation = FVector(-60.f, -40.f, 0.f);
};
