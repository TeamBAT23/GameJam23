#include "RootTargetType.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "GGJ23/RootCharacter.h"

void URootTargetType::GetTargets_Implementation(ARootCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return; //by default return no targets
}

void URootTargetType_UseOwner::GetTargets_Implementation(ARootCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void URootTargetType_UseEventData::GetTargets_Implementation(ARootCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (const FHitResult* HitResult = EventData.ContextHandle.GetHitResult())
	{
		OutHitResults.Add(*HitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target.Get()));
	}
}