#pragma once

#include "RootTargetType.generated.h"

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class GGJ23_API URootTargetType : public UObject
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	URootTargetType() {}

	/** Called to determine targets to apply gameplay effects to. Returns a list of hit results and actors (to target) */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(class ARootCharacter* TargetingCharacter, AActor* TargetingActor, struct FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class GGJ23_API URootTargetType_UseOwner : public URootTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	URootTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ARootCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class GGJ23_API URootTargetType_UseEventData : public URootTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	URootTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ARootCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
