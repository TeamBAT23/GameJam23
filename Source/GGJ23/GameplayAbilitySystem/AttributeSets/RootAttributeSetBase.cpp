#include "RootAttributeSetBase.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

bool URootAttributeSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(OnPreGameplayEffectExecute.IsBound())
	{
		FGameplayEffectMulticastPayload Payload;
		Payload.EffectSpec = Data.EffectSpec;
		Payload.EvaluatedData = Data.EvaluatedData;
		Payload.Target = &Data.Target;
		OnPreGameplayEffectExecute.Broadcast(this, Payload);
	}
	return true;
}

void URootAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if(OnPostGameplayEffectExecute.IsBound())
	{
		FGameplayEffectMulticastPayload Payload;
		Payload.EffectSpec = Data.EffectSpec;
		Payload.EvaluatedData = Data.EvaluatedData;
		Payload.Target = &Data.Target;
		OnPostGameplayEffectExecute.Broadcast(this, Payload);
	}
}

void URootAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if(ShouldSanitizeCurrentValue(Attribute))
	{
		ReapplyAffectedSanitizers(Attribute, NewValue);
	}
	OnPreAttributeChange.Broadcast(this, Attribute, NewValue);
}

void URootAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	OnPostAttributeChange.Broadcast(this, Attribute, NewValue, OldValue);
}

void URootAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	ReapplyAffectedSanitizers(Attribute, NewValue);
	
	OnPreAttributeBaseChange.Broadcast(this, Attribute, NewValue);
}

void URootAttributeSetBase::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	OnPostAttributeBaseChange.Broadcast(this, Attribute, NewValue, OldValue);
}

void URootAttributeSetBase::ReapplyAffectedSanitizers(const FGameplayAttribute& Attribute, float& NewValue) const
{
	const FProperty* Property = Attribute.GetUProperty();
	const FString HandlerKey = GetSanitizerKey(Property->GetName());
	if(!GetAttributeSanitizers().Contains(HandlerKey))
	{
		//apply any sanitizers to the property
		SanitizeAttributeValue(Attribute, NewValue);
	}
	else
	{
		//This is a sanitizer attribute. Get the Source Property and apply sanitizers
		//Handle in Post Attribute Change
		FString AttrName = ChopSanitizerKey(Property->GetName());
		FProperty* TargetProp = GetClass()->FindPropertyByName(FName{AttrName});
		if(FGameplayAttribute::IsGameplayAttributeDataProperty(TargetProp))
		{
			SanitizeAttributeValue( FGameplayAttribute{TargetProp}, NewValue);
		}
	}
}

void URootAttributeSetBase::SanitizeAttributeValue(const FGameplayAttribute& Attribute, float& NewValue) const
{
	const FProperty* Property = Attribute.GetUProperty();
	check(Property);

	//Go to next property looking for sanitizer (max, min etc). Apply them until no more sanitizer found
	const FString SanitizerName = Property->GetName() + TEXT("_");
	for(FProperty* Sanitizer = Property->PropertyLinkNext;Sanitizer && Sanitizer->GetName().StartsWith(SanitizerName);Sanitizer = Sanitizer->PropertyLinkNext)
	{
		if(!FGameplayAttribute::IsGameplayAttributeDataProperty(Sanitizer))
		{
			break; //not a handler attribute
		}

		//See if there is a corresponding sanitizer stored as the next property
		FGameplayAttribute SanitizerAttribute{Sanitizer};
		FString Key = GetSanitizerKey(Sanitizer->GetName());
		if(const auto Handler = GetAttributeSanitizers().Find(Key))
		{
			NewValue = (*Handler)(this, SanitizerAttribute, NewValue);
		}
		else
		{
			break; // End of handlers
		}
	}
}

void URootAttributeSetBase::GetSanitizersForAttribute(FGameplayAttribute InAttribute, TArray<FGameplayAttribute>& OutSanitizers) const
{
	OutSanitizers.Empty();
	if(InAttribute.IsValid())
	{
		const FProperty* Property = InAttribute.GetUProperty();
		const FString SanitizerPrefix = Property->GetName() + TEXT("_");
		for(FProperty* Sanitizer = Property->PropertyLinkNext;Sanitizer && Sanitizer->GetName().StartsWith(SanitizerPrefix);Sanitizer = Sanitizer->PropertyLinkNext)
		{
			if(!FGameplayAttribute::IsGameplayAttributeDataProperty(Sanitizer) &&
				GetAttributeSanitizers().Contains(GetSanitizerKey(Sanitizer->GetName())))
			{
				break; //not a handler attribute
			}

			OutSanitizers.Emplace(Sanitizer);
		}
	}
}

FGameplayAttribute URootAttributeSetBase::GetAttributeSanitizerByName(const FGameplayAttribute& Attribute, const FString& SanitizerName)
{
	FGameplayAttribute Result;
	if(Attribute.IsValid())
	{
		const FName Key{Attribute.GetName() + TEXT("_") + SanitizerName};
		FProperty* Property = GetClass()->FindPropertyByName(Key);
		if(Property && FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			return FGameplayAttribute{Property};
		}
	} 
	return FGameplayAttribute{};
}

FGameplayAttribute URootAttributeSetBase::GetAttributeByName(const FString& Name)
{
	return FGameplayAttribute{GetClass()->FindPropertyByName(FName{Name})};		
}

void URootAttributeSetBase::SetFloatValue(const FGameplayAttribute& Attribute, float NewValue)
{
	if(Attribute.IsValid())
	{
		Attribute.SetNumericValueChecked(NewValue, this);
	}
}

float URootAttributeSetBase::GetFloatValue(const FGameplayAttribute& Attribute)
{
	if(Attribute.IsValid())
	{
		return Attribute.GetNumericValueChecked(this);
	}
	return 0.0f;
}

FString URootAttributeSetBase::GetSanitizerKey(const FString& PropName)
{
	return PropName.RightChop(PropName.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) + 1);
}

FString URootAttributeSetBase::ChopSanitizerKey(const FString& PropName)
{
	return PropName.Left(PropName.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) + 1);
}

TMap<FString, FRootAttrSanitizerSignature>& URootAttributeSetBase::GetAttributeSanitizers()
{
	static TMap<FString, FRootAttrSanitizerSignature> Handlers = {
		{TEXT("Min"), &URootAttributeSetBase::Sanitizer_ApplyMin},
		{TEXT("Max"), &URootAttributeSetBase::Sanitizer_ApplyMax}
	};
	return Handlers;
}

float URootAttributeSetBase::Sanitizer_ApplyMin(const UAttributeSet* Target, const FGameplayAttribute& MinAttribute,
	float InValue)
{
	return FMath::Max(InValue, MinAttribute.GetNumericValue(Target));
}

float URootAttributeSetBase::Sanitizer_ApplyMax(const UAttributeSet* Target, const FGameplayAttribute& MaxAttribute,
	float InValue)
{
	return FMath::Min(InValue, MaxAttribute.GetNumericValue(Target));
}

void URootAttributeSetBase::GetReplicatedGameplayProperties(const UClass* Class, TArray<FLifetimeProperty>& OutLifetimeProps, bool bIncludeSuper) const
{
	for(TFieldIterator<FProperty> It(Class, bIncludeSuper ? EFieldIterationFlags::IncludeSuper : EFieldIterationFlags::None);It;++It)
	{
		if(It->PropertyFlags & CPF_Net && FGameplayAttribute::IsGameplayAttributeDataProperty(*It))
		{
			AddReplicatedProperty(OutLifetimeProps, *It, COND_None, REPNOTIFY_Always);
		}
	}
}

void URootAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Register all Gameplay Attribute Properties automatically for replication
	GetReplicatedGameplayProperties(GetClass(), OutLifetimeProps, true);
}


void URootAttributeSetBase::AddReplicatedProperty(TArray<FLifetimeProperty>& OutLifetimeProps, const FProperty* Property, ELifetimeCondition Condition = COND_None, ELifetimeRepNotifyCondition RepNotifyCondition = REPNOTIFY_Always) const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (!(Property->PropertyFlags & CPF_Net))
	{
		UE_LOG(LogNet, Fatal,TEXT("Attempt to replicate property '%s' that was not tagged to replicate! Please use 'Replicated' or 'ReplicatedUsing' keyword in the UPROPERTY() declaration."), *Property->GetFullName());
		return;
	}
#endif
	RegisterReplicatedLifetimeProperty(Property, OutLifetimeProps, FDoRepLifetimeParams{Condition, RepNotifyCondition});
}