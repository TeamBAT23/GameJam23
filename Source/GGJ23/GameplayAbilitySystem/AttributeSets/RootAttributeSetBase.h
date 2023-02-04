#pragma once

#include "AttributeSet.h"
#include "AttributeSetMacros.h"

#include "RootAttributeSetBase.generated.h"

using FRootAttrSanitizerSignature = TFunction<float(const UAttributeSet*, const FGameplayAttribute&, float)>;

USTRUCT(BlueprintType)
struct GGJ23_API FGameplayEffectMulticastPayload
{
	GENERATED_BODY()

	/** The spec that the mod came from */
	UPROPERTY(BlueprintReadOnly)
	struct FGameplayEffectSpec EffectSpec;

	/** The 'flat'/computed data to be applied to the target */
	UPROPERTY(BlueprintReadWrite)
	struct FGameplayModifierEvaluatedData EvaluatedData;

	/** Target we intend to apply to */
	UPROPERTY(BlueprintReadOnly)
	class UAbilitySystemComponent* Target;		
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPreGameplayEffectExecute, class URootAttributeSetBase*, AttributeSet, const FGameplayEffectMulticastPayload&, Attribute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPostGameplayEffectExecute, class URootAttributeSetBase*, AttributeSet, const FGameplayEffectMulticastPayload&, Attribute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPreAttributeChangeEvent, class URootAttributeSetBase*, AttributeSet, const FGameplayAttribute&, Attribute, float&, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostAttributeChangeEvent, class URootAttributeSetBase*, AttributeSet, const FGameplayAttribute&, Attribute, float, NewValue, float, OldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPreAttributeBaseChangeEvent, const class URootAttributeSetBase*, AttributeSet, const FGameplayAttribute&, Attribute, float&, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostAttributeBaseChangeEvent, const class URootAttributeSetBase*, AttributeSet, const FGameplayAttribute&, Attribute, float, NewValue, float, OldValue);

UCLASS(Abstract, BlueprintType)
class GGJ23_API URootAttributeSetBase: public UAttributeSet
{
	GENERATED_BODY()
public:
	//--------- Multicast delegates exposed to blueprints ------------
	/** Event raised just before a gameplay effect is applied to an attribute.*/
	UPROPERTY(BlueprintAssignable)
	FPreGameplayEffectExecute OnPreGameplayEffectExecute;
	
	/** Event raised just before a gameplay effect is applied to an attribute.*/
	UPROPERTY(BlueprintAssignable)
	FPostGameplayEffectExecute OnPostGameplayEffectExecute;

	/**
	 * Event raised just before modification occurs to value of an attribute.
	 * Base value is attribute value after modifiers applied.
	 */
	UPROPERTY(BlueprintAssignable)
	FPreAttributeChangeEvent OnPreAttributeChange;

	/**
	 * Event raised just after modification occurs to value of an attribute.
	 * Base value is attribute value after modifiers applied.
	 */
	UPROPERTY(BlueprintAssignable)
	FPostAttributeChangeEvent OnPostAttributeChange;

	/**
	 * Event raised just before modification occurs to the base value of an attribute.
	 * Base value is attribute value before modifiers applied.
	 */
	UPROPERTY(BlueprintAssignable)
	FPreAttributeBaseChangeEvent OnPreAttributeBaseChange;

	/**
	 * Event raised just after modification occurs to the base value of an attribute.
	 * Base value is attribute value before modifiers applied.
	 */
	UPROPERTY(BlueprintAssignable)
	FPostAttributeBaseChangeEvent OnPostAttributeBaseChange;

public:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	/**
	 * Call this to apply any modifiers.
	 * This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	 */
	void ReapplyAffectedSanitizers(const FGameplayAttribute& Attribute, float& NewValue) const;

	/**
	 * Apply any effects of any adjacent attribute sanitizers to a target attribute. Done on pre and post attribute change
	 * Sanitizers must be declared immediately. Name should be of form [AttrToModify]_[ModifierKey]
	 * @see GetSanitizersForAttribute
	 * Example: Health = Clamp(Health, Health_Min, Health)
	 */
	void SanitizeAttributeValue(const FGameplayAttribute& Attribute, float& NewValue) const;
	
	/** Get all Gameplay Attributes that act as modifiers for a given attribute*/
	UFUNCTION(BlueprintCallable)
	void GetSanitizersForAttribute(FGameplayAttribute InAttribute, TArray<FGameplayAttribute>& OutSanitizers) const;

	/** Get a modifier for an attribute given a name to append to base attribute.*/
	UFUNCTION(BlueprintCallable)
	FGameplayAttribute GetAttributeSanitizerByName(const FGameplayAttribute& Attribute, const FString& SanitizerName);

	UFUNCTION(BlueprintCallable)
	FGameplayAttribute GetAttributeByName(const FString& Name);

	UFUNCTION(BlueprintCallable)
	void SetFloatValue(const FGameplayAttribute& Attribute, float NewValue);

	UFUNCTION(BlueprintCallable)
	float GetFloatValue(const FGameplayAttribute& Attribute);

	/** Takes a property name and slices off the end. "Health_Min" -> "Min"*/
	static FString GetSanitizerKey(const FString& PropName);
	
	/** Takes a property name and slices off the end (the key). "Health_Min" -> "Health"*/
	static FString ChopSanitizerKey(const FString& PropName);

	static TMap<FString, FRootAttrSanitizerSignature>& GetAttributeSanitizers();

	/** Clamp value to be greater than or equal than MinAttribute*/
	static float Sanitizer_ApplyMin(const UAttributeSet* Target, const FGameplayAttribute& MinAttribute, float InValue);

	/** Clamp value to be less than or equal to MaxAttribute*/
	static float Sanitizer_ApplyMax(const UAttributeSet* Target, const FGameplayAttribute& MaxAttribute, float InValue);

	//Network Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void GetReplicatedGameplayProperties(const UClass* Class, TArray<FLifetimeProperty>& OutLifetimeProps, bool bIncludeSuper = true) const;
	
	/** Add a replicated property. Used to automatically harvest all properties that want replication*/
	void AddReplicatedProperty(TArray<FLifetimeProperty>& OutLifetimeProps, const FProperty* Property, ELifetimeCondition Condition, ELifetimeRepNotifyCondition
							   RepNotifyCondition) const;


	/** Return true to apply sanitizers to current value in addition to base value*/
	virtual bool ShouldSanitizeCurrentValue(const FGameplayAttribute& Attribute){return false;}
};