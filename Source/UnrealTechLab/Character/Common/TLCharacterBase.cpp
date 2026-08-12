// Fill out your copyright notice in the Description page of Project Settings.


#include "TLCharacterBase.h"

ATLCharacterBase::ATLCharacterBase()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	
}

void ATLCharacterBase::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s is dead"), *GetOwner()->GetName());
}
