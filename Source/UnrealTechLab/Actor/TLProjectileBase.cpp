#include "TLProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ATLProjectileBase::ATLProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
		SetRootComponent(CollisionComponent);
	
		CollisionComponent->InitSphereRadius(5.0f);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	}

	{
		ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
		ProjectileMesh->SetupAttachment(CollisionComponent);
		ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	{
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
		ProjectileMovement->UpdatedComponent = CollisionComponent;
		ProjectileMovement->InitialSpeed = 3000.0f;
		ProjectileMovement->MaxSpeed = 3000.0f;
		ProjectileMovement->ProjectileGravityScale = 1.0f;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = false;
	}
}

void ATLProjectileBase::InitializeProjectile(UAbilitySystemComponent* InSourceASC, const FGameplayEffectSpecHandle& InDamageSpecHandle)
{
	SourceASC = InSourceASC;
	DamageSpecHandle = InDamageSpecHandle;
}

void ATLProjectileBase::SetHomingTarget(AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return;
	}
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = TargetActor->GetRootComponent();
}

void ATLProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentHit.AddDynamic(this,	&ThisClass::OnProjectileHit);

	SetLifeSpan(LifeTime);

	// 발사한 액터와 즉시 충돌하지 않도록 한다.
	if (AActor* ProjectileOwner = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(ProjectileOwner,true);
	}

	if (APawn* InstigatorPawn = GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(InstigatorPawn,true);
	}
}

void ATLProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority() || !OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (!SourceASC || !TargetASC || !DamageSpecHandle.IsValid())
	{
		Destroy();
		return;
	}

	DamageSpecHandle.Data->GetContext().AddHitResult(Hit, true);
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);

	Destroy();
}