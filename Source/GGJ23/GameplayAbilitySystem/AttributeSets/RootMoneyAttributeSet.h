#pragma once

#include "RootAttributeSetBase.h"

#include "RootMoneyAttributeSet.generated.h"

UCLASS(BlueprintType)
class GGJ23_API URootMoneyAttributeSet : public URootAttributeSetBase
{
	GENERATED_BODY()
public:
	/** Money */ 
    UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Money)
    FGameplayAttributeData Money;
    ATTRIBUTE_ACCESSORS(ThisClass, Money);
    
    UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Money_Min)
    FGameplayAttributeData Money_Min = {0};
    ATTRIBUTE_ACCESSORS(ThisClass, Money_Min);
    
    /** Money_Max */ 
    UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing= OnRep_Money_Max)
    FGameplayAttributeData Money_Max = {50};
    ATTRIBUTE_ACCESSORS(ThisClass, Money_Max);
    
    /** @see Money*/
    UFUNCTION()
    virtual void OnRep_Money(const FGameplayAttributeData& OldMoney)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Money, OldMoney);
    }
    
    /** @see Money_Min*/
    UFUNCTION()
    virtual void OnRep_Money_Min(const FGameplayAttributeData& OldMoney_Min)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Money_Min, OldMoney_Min);
    }
    
    /** @see Money_Max*/
    UFUNCTION()
    virtual void OnRep_Money_Max(const FGameplayAttributeData& OldMoney_Max)
    {
    	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Money_Max, OldMoney_Max);
    }

	//Network Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
    	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    }
};
