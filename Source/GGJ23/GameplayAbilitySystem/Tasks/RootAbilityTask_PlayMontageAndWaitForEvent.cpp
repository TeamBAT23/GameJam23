#include "RootAbilityTask_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "GameFramework/Character.h"

void URootAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	UAbilitySystemComponent* AbilityComponent = AbilitySystemComponent.Get();
	UAnimInstance* AnimInstance = IsValid(Ability) ? Ability->GetCurrentActorInfo()->GetAnimInstance() : nullptr;
	if(!IsValid(Ability) || !IsValid(AnimInstance) || !IsValid(AbilityComponent))
	{
		ABILITY_LOG(Warning, TEXT("URootAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
		return;
	}

	//Bind to gameplay event before playing montage
	GameplayEventHandle = AbilityComponent->AddGameplayEventTagContainerDelegate(EventTags,
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &URootAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));
		
	if(AbilityComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, StartTimeSeconds) > 0.0f)
	{
		//Early return if the ability has been canceled already
		if(ShouldBroadcastAbilityTaskDelegates() == false)
		{
			return;
		}

		CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &URootAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

		BlendingOutDelegate.BindUObject(this, &URootAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);
			
		MontageEndedDelegate.BindUObject(this, &URootAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, MontageToPlay);

		SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("URootAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
		
	SetWaitingOnAvatar();
}

void URootAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString URootAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void URootAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}
		
	if (UAbilitySystemComponent* AbilityComponent = AbilitySystemComponent.Get())
	{
		AbilityComponent->RemoveGameplayEventTagContainerDelegate(EventTags, GameplayEventHandle);
	}
		
	Super::OnDestroy(AbilityEnded);
}

URootAbilityTask_PlayMontageAndWaitForEvent* URootAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay,
	FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale, float StartTimeSeconds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	URootAbilityTask_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<URootAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}

bool URootAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage() const
{
	const FGameplayAbilityActorInfo* ActorInfo = IsValid(Ability) ? Ability->GetCurrentActorInfo() : nullptr;
	UAnimInstance* AnimInstance = ActorInfo ? ActorInfo->GetAnimInstance() : nullptr;
		
	if (!IsValid(AnimInstance) ||
		!AbilitySystemComponent.IsValid() ||
		AbilitySystemComponent->GetAnimatingAbility() != Ability ||
		AbilitySystemComponent->GetCurrentMontage() != MontageToPlay)
	{
		return false;
	}

	// Unbind delegates so they don't get called as well
	if(FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
	{
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();
	}
	AbilitySystemComponent->CurrentMontageStop();
	return true;
}

void URootAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled() const
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void URootAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void URootAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	//Reset any state
	if (Ability &&
		Ability->GetCurrentMontage() == MontageToPlay
		&& MontageToPlay == Montage)
	{
		AbilitySystemComponent->ClearAnimatingAbility(Ability);
		SetAnimRootMotionTranslationScale(1.0f);
	}

	//Raise Events
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		if(bInterrupted)
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
		else
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void URootAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag,
	const FGameplayEventData* Payload) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

void URootAbilityTask_PlayMontageAndWaitForEvent::SetAnimRootMotionTranslationScale(float Value) const
{
	if(ACharacter* Character = Cast<ACharacter>(GetAvatarActor()))
	{
		const bool bAffectRootMotion = Character->GetLocalRole() == ROLE_Authority ||
			Character->GetLocalRole() == ROLE_AutonomousProxy &&
			Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted;
		if(bAffectRootMotion)
		{
			Character->SetAnimRootMotionTranslationScale(Value);
		}
	}
}
