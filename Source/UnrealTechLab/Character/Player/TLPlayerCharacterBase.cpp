// Fill out your copyright notice in the Description page of Project Settings.


#include "TLPlayerCharacterBase.h"

#include "EnhancedInputSubsystems.h"
#include "TLGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Input/TLInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "State/TLPlayerState.h"

ATLPlayerCharacterBase::ATLPlayerCharacterBase()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>("First Person Mesh");
	FPMesh->SetupAttachment(Camera);
}

void ATLPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);
	
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	Subsystem->ClearAllMappings();

	const UInputMappingContext* IMC = InputMappingContext.LoadSynchronous();
					
	// 우선순위가 높은 입력 매핑이 낮은 입력 매핑보다 우선적으로 처리됩니다.
	Subsystem->AddMappingContext(IMC, 0);
	
	UTLInputComponent* IC = CastChecked<UTLInputComponent>(PlayerInputComponent);
	
	IC->BindNativeAction(InputConfig, TLGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	IC->BindNativeAction(InputConfig, TLGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);

	IC->BindAbilityAction(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

void ATLPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
}

void ATLPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
}

void ATLPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BowAnimState);
}

void ATLPlayerCharacterBase::SetBowAnimState(ETLBowAnimState NewState)
{
	if (BowAnimState == NewState)
	{
		return;
	}

	BowAnimState = NewState;
	OnRep_BowAnimState();

	if (HasAuthority())
	{
		ForceNetUpdate();
	}
}

void ATLPlayerCharacterBase::OnRep_BowAnimState()
{
	// ABP_Bow에서 GetBowAnimState()를 읽도록 두고, 추후 시각 요소 갱신이 필요하면 여기에 추가합니다.
}

void ATLPlayerCharacterBase::SetMeshPerspective(bool bFirstPerson)
{
	GetMesh()->SetOwnerNoSee(bFirstPerson);
	FPMesh->SetVisibility(bFirstPerson, true);

	Camera->SetRelativeLocation(bFirstPerson ? FirstPersonCameraLocation : ThirdPersonCameraLocation);
}

void ATLPlayerCharacterBase::InitAbilityActorInfo()
{
	ATLPlayerState* PS = GetPlayerState<ATLPlayerState>();
	if (!PS)
	{
		return;
	}

	ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	ASC->InitAbilityActorInfo(PS, this);
}

void ATLPlayerCharacterBase::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ATLPlayerCharacterBase::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	
	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void ATLPlayerCharacterBase::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!ASC)
	{
		return;
	}
	
	ASC->AbilityInputTagPressed(InputTag);
}

void ATLPlayerCharacterBase::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!ASC)
	{
		return;
	}
	
	ASC->AbilityInputTagReleased(InputTag);
}

USkeletalMeshComponent* ATLPlayerCharacterBase::GetFirstPersonMesh()
{
	return FPMesh ? FPMesh : nullptr;
}
