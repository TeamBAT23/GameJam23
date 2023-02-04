// Authored by Andrew Case

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RootAbilitiesData.h"

#include "RootGameplayAbility.generated.h"

/**
 * Struct defining a list of gameplay effects, a tag, and targeting info
 * These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 */
USTRUCT(BlueprintType)
struct FRootTargetedEffectSpec
{
	GENERATED_BODY()

public:
	FRootTargetedEffectSpec() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<class URootTargetType> TargetingSystem;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
};

USTRUCT(BlueprintType)
struct GGJ23_API FRootTargetedEffectHandle
{
	GENERATED_BODY()

	/** Computed target data. Contains a composition of Hit Results and Actors.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> GameplayEffects;
	
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	bool HasValidEffects() const{ return GameplayEffects.Num() > 0;}

	bool HasValidTargets() const { return TargetData.Num() > 0;}
};

UCLASS()
class GGJ23_API URootGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	ERootAbilityInputId GetInputId() const{return AbilityInputId;}

	/** Make an targeted effect spec handle using spec. Targeting will be performed and the result returned as a handle.*/
	UFUNCTION(BlueprintCallable)
	FRootTargetedEffectHandle MakeTargetedEffectFromSpec(const FRootTargetedEffectSpec& Spec, const FGameplayEventData& EventData, int OverrideGameplayLevel = -1) const;

	/** Make an targeted effect spec handle using a gameplay tag as a lookup. Uses the TagToEffectSpecMap to look up a tag using an exact match.*/
	UFUNCTION(BlueprintCallable)
	FRootTargetedEffectHandle MakeTargetedEffectFromTag(FGameplayTag EffectTag, const FGameplayEventData& EventData, int OverrideGameplayLevel = -1);
protected:
	/** Map of gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FRootTargetedEffectSpec> TagToEffectSpecMap;

	/**
	 * Input event to associate with this ability when given.
	 * Each value in the enum has a matching action input in the project settings*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	ERootAbilityInputId AbilityInputId = ERootAbilityInputId::None;
};
