#pragma once
#include "Abilities/Tasks/AbilityTask.h"

#include "RootAbilityTask_PlayMontageAndWaitForEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRootPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * This task combines PlayMontageAndWait and WaitForEvent into one task, so you can wait for multiple types of activations such as from a melee combo
 */
UCLASS()
class GGJ23_API URootAbilityTask_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	/** The montage completely finished playing */
	UPROPERTY(BlueprintAssignable)
	FRootPlayMontageAndWaitForEventDelegate OnCompleted;

	/** The montage started blending out */
	UPROPERTY(BlueprintAssignable)
	FRootPlayMontageAndWaitForEventDelegate OnBlendOut;

	/** The montage was interrupted */
	UPROPERTY(BlueprintAssignable)
	FRootPlayMontageAndWaitForEventDelegate OnInterrupted;

	/** The ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
	FRootPlayMontageAndWaitForEventDelegate OnCancelled;

	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FRootPlayMontageAndWaitForEventDelegate EventReceived;

private:
	/** Montage that is playing */
	UPROPERTY()
	UAnimMontage* MontageToPlay = nullptr;

	/** List of tags to match against gameplay events */
	UPROPERTY()
	FGameplayTagContainer EventTags = FGameplayTagContainer{};

	/** Playback rate */
	UPROPERTY()
	float Rate =  1.0f;

	/** Section to start montage from */
	UPROPERTY()
	FName StartSection = NAME_None;

	/** Time to start the montage from */
	UPROPERTY()
	float StartTimeSeconds = 0.0f;

	/** Modifies how root motion movement to apply */
	UPROPERTY()
	float AnimRootMotionTranslationScale = 1.0f;

	/** Rather montage should be aborted if ability ends */
	UPROPERTY()
	bool bStopWhenAbilityEnds = true;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle GameplayEventHandle;
public:
	//Ability Task Overrides
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;
	//End Ability Task Overrides

	/**
	 * Play a montage and wait for it end. If a gameplay event happens that matches EventTags (or EventTags is empty), the EventReceived delegate will fire with a tag and event data.
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param InMontageToPlay The montage to play on the character
	 * @param InEventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param InRate Change to play the montage faster or slower
	 * @param InStartSection If not empty, named montage section to start from
	 * @param StopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param InAnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	* @param InStartTimeSeconds Starting time offset in montage, this will be overridden by StartSection if that is also set
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URootAbilityTask_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* InMontageToPlay,
		FGameplayTagContainer InEventTags,
		float InRate = 1.f,
		FName InStartSection = NAME_None,
		bool StopWhenAbilityEnds = true,
		float InAnimRootMotionTranslationScale = 1.f,
		float InStartTimeSeconds = 0.f);
	
private:
	/**
	 * Checks if the ability is playing a montage and stops that montage.
	 * @return true if a montage was stopped
	 */
	bool StopPlayingMontage() const;

	/** Callback for when the ability is canceled*/
	void OnAbilityCancelled() const;

	/** Callback for when the montage ends*/
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Callback for when the montage starts to blend out*/
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Callback for when a gameplay event is received */
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const;
	
	/** Helper function for Updating the character's root motion translation scale. Performs check for net authority.*/
	void SetAnimRootMotionTranslationScale(float Value) const;
};
