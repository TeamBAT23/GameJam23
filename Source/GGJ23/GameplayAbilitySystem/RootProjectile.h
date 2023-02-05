#pragma once
#include "RootGameplayAbility.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "RootProjectile.generated.h"

UCLASS(Blueprintable)
class GGJ23_API ARootProjectile : public AActor
{
	GENERATED_BODY()
public:
	/** Handle to struct containing any target information plus any effects to apply.*/
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn))
	FRootTargetedEffectHandle EffectHandle;

	ARootProjectile()
	{
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile Movement")
	UProjectileMovementComponent* ProjectileMovement;
};
