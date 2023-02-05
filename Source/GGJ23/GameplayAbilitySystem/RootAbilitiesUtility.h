#pragma once
#include "RootGameplayAbility.h"

#include "RootAbilitiesUtility.generated.h"

UCLASS()
class GGJ23_API URootAbilitiesUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FRootTargetedEffectHandle MakeTargetedEffect(const TArray<FGameplayEffectSpecHandle>& Effects, const TArray<AActor*>& TargetActors, const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable, Category = Ability)
	static FRootTargetedEffectHandle AddEffectsToTargetedEffect(const FRootTargetedEffectHandle& InContainerSpec, const TArray<FGameplayEffectSpecHandle>& Effects);

	/**
	 * Adds targets to a targeted effect
	 * @return A copy of the targeted effect with additional targets added
	 */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FRootTargetedEffectHandle AddTargetsToTargetedEffect(const FRootTargetedEffectHandle& InContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability.
	 * @param TargetedEffect Spec describing a list of effects and list of targets
	 * @return list of handles to applied gameplay effects
	 */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyTargetedEffect(const FRootTargetedEffectHandle& TargetedEffect);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Ability)
	static bool DoesTargetedEffectHaveValidEffects(const FRootTargetedEffectHandle& TargetedEffect);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Ability)
	static bool DoesTargetedEffectHaveValidTargets(const FRootTargetedEffectHandle& TargetedEffect);
};
