// Authored by Andrew Case

#include "RootGameplayAbility.h"
#include "RootTargetType.h"
#include "GGJ23/RootCharacter.h"

void FRootTargetedEffectHandle::AddTargets(const TArray<FHitResult>& HitResults,
                                           const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
	}
	
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

FRootTargetedEffectHandle URootGameplayAbility::MakeTargetedEffectFromSpec(
	const FRootTargetedEffectSpec& Spec, const FGameplayEventData& EventData,
	int OverrideGameplayLevel) const
{
	FRootTargetedEffectHandle ReturnSpec;
		
	if(ARootCharacter* OwningActor = Cast<ARootCharacter>(GetOwningActorFromActorInfo()))
	{
		if(const URootTargetType* CDO = Spec.TargetingSystem.GetDefaultObject())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			CDO->GetTargets(OwningActor, GetAvatarActorFromActorInfo(), EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		OverrideGameplayLevel = OverrideGameplayLevel == INDEX_NONE ? GetAbilityLevel() : OverrideGameplayLevel;

		for(const TSubclassOf<UGameplayEffect>& EffectClass : Spec.GameplayEffects)
		{
			ReturnSpec.GameplayEffects.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}

	return ReturnSpec;
}

FRootTargetedEffectHandle URootGameplayAbility::MakeTargetedEffectFromTag(FGameplayTag EffectTag,
	const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	if(const FRootTargetedEffectSpec* FoundContainer = TagToEffectSpecMap.Find(EffectTag))
	{
		return MakeTargetedEffectFromSpec(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FRootTargetedEffectHandle{};
}
