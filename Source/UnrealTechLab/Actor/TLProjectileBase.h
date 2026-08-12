#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "TLProjectileBase.generated.h"

class UGameplayEffect;
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class UNREALTECHLAB_API ATLProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	ATLProjectileBase();

	void InitializeProjectile(UAbilitySystemComponent* InSourceASC, const FGameplayEffectSpecHandle& InDamageSpecHandle);
	void SetHomingTarget(AActor* TargetActor = nullptr);
	UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovement; };

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,	const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float LifeTime = 5.0f;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	FGameplayEffectSpecHandle DamageSpecHandle;
};