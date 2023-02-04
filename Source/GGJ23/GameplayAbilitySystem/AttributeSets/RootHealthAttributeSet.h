#pragma once
#include "RootAttributeSetBase.h"

#include "RootHealthAttributeSet.generated.h"

UCLASS(BlueprintType)
class GGJ23_API URootHealthAttributeSet : public URootAttributeSetBase
{
GENERATED_BODY()
public:
	/** Health */ 
	UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health);

	/** Health_Min */ 
	UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Health_Min)
	FGameplayAttributeData Health_Min = {0.0f};
	ATTRIBUTE_ACCESSORS(ThisClass, Health_Min);

	/** Health_Max */ 
	UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Health_Max)
	FGameplayAttributeData Health_Max = {5.0f};
	ATTRIBUTE_ACCESSORS(ThisClass, Health_Max);
	
	/** @see Health*/
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldHealth);
    }
    
    /** @see Health_Min*/
    UFUNCTION()
    virtual void OnRep_Health_Min(const FGameplayAttributeData& OldHealth_Min)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health_Min, OldHealth_Min);
    }
    
    /** @see Health_Max*/
    UFUNCTION()
    virtual void OnRep_Health_Max(const FGameplayAttributeData& OldHealth_Max)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health_Max, OldHealth_Max);
    }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
	    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    }
};
