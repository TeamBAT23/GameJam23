#include "RootAbilitiesUtility.h"

FRootTargetedEffectHandle URootAbilitiesUtility::MakeTargetedEffect(const TArray<FGameplayEffectSpecHandle>& Effects,
                                                                     const TArray<AActor*>& TargetActors,
                                                                     const TArray<FHitResult>& HitResults)
{
	FRootTargetedEffectHandle ReturnSpec{};
	ReturnSpec.GameplayEffects.Append(Effects);
	ReturnSpec.AddTargets(HitResults, TargetActors);
	return ReturnSpec;
}

FRootTargetedEffectHandle URootAbilitiesUtility::AddEffectsToTargetedEffect(
	const FRootTargetedEffectHandle& InContainerSpec, const TArray<FGameplayEffectSpecHandle>& Effects)
{
	FRootTargetedEffectHandle ReturnSpec = InContainerSpec;
	ReturnSpec.GameplayEffects.Append(Effects);
	return ReturnSpec;
}

FRootTargetedEffectHandle URootAbilitiesUtility::AddTargetsToTargetedEffect(
	const FRootTargetedEffectHandle& InContainerSpec, const TArray<FHitResult>& HitResults,
	const TArray<AActor*>& TargetActors)
{
	FRootTargetedEffectHandle Copy = InContainerSpec;
	Copy.AddTargets(HitResults, TargetActors);
	return Copy;
}

TArray<FActiveGameplayEffectHandle> URootAbilitiesUtility::ApplyTargetedEffect(
	const FRootTargetedEffectHandle& TargetedEffect)
{
	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	for(const FGameplayEffectSpecHandle& SpecHandle : TargetedEffect.GameplayEffects)
	{
		if(SpecHandle.IsValid())
		{
			for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetedEffect.TargetData.Data)
			{
				AppliedEffects.Append(TargetData->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}

	return AppliedEffects;
}

bool URootAbilitiesUtility::DoesTargetedEffectHaveValidEffects(const FRootTargetedEffectHandle& TargetedEffect)
{
	return TargetedEffect.HasValidEffects();
}

bool URootAbilitiesUtility::DoesTargetedEffectHaveValidTargets(const FRootTargetedEffectHandle& TargetedEffect)
{
	return TargetedEffect.HasValidTargets();
}
