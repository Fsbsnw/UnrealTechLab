// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/TLAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "TLCharacterBase.generated.h"

UCLASS()
class UNREALTECHLAB_API ATLCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ATLCharacterBase();

// ─────────────────────────────────────────────────────────────
// AbilitySystem Interface
// ─────────────────────────────────────────────────────────────		
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

protected:
	UPROPERTY()
	TObjectPtr<UTLAbilitySystemComponent> ASC;

// ─────────────────────────────────────────────────────────────
// Death State Method
// ─────────────────────────────────────────────────────────────	
protected:
	UFUNCTION()
	virtual void OnDeath();
};