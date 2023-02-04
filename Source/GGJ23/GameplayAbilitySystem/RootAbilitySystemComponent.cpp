// Authored by Andrew Case
#include "RootAbilitySystemComponent.h"

#include "GGJ23/Utility/RootUtility.h"

void URootAbilitySystemComponent::OnRegister()
{
	DefaultStartingData.Empty();
	Super::OnRegister();

	if(URootUtility::IsGameRunning(this))
	{
		if(GetOwner()->GetNetMode() < ENetMode::NM_Client)
		{
			ServerInit();
		}
		else{
			ClientInit();
		}
	}
}

void URootAbilitySystemComponent::ClientInit()
{
	GiveDefaultAttributes();
}

void URootAbilitySystemComponent::ServerInit()
{
	GiveDefaultAttributes();
	GiveDefaultAbilities();
}

void URootAbilitySystemComponent::GiveDefaultAttributes()
{
	if(DefaultAttributes.Num () == 0)
	{
		return;
	}
		
	TSet<TSubclassOf<UAttributeSet>> AttributeSetsToAdd = {};

	//Collect all attribute effects that are referenced in DefaultAttributes
	for(const FDefaultAttributeSpec& DefaultAttributeSpec : DefaultAttributes)
	{
		if(!IsValid(DefaultAttributeSpec.InitializationEffect))
		{
			continue;
		}

		if(const UGameplayEffect* CDO = DefaultAttributeSpec.InitializationEffect.GetDefaultObject())
		{
			CollectAffectAttributeSets(CDO, AttributeSetsToAdd);
		}
	}

	//Add any attribute sets that are used by the default initialization effects
	for(TSubclassOf<UAttributeSet> AttributeSet : AttributeSetsToAdd)
	{
		GetOrCreateAttributeSubobject(AttributeSet);
	}

	//Apply initialization effects for all default attributes
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(GetOwner());
	for(const FDefaultAttributeSpec& DefaultAttribute : DefaultAttributes)
	{
		if(!IsValid(DefaultAttribute.InitializationEffect))
		{
			continue;
		}
		
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DefaultAttribute.InitializationEffect,
		                                                        GetStartingLevel(DefaultAttribute),
		                                                        EffectContext);
		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void URootAbilitySystemComponent::GiveDefaultAbilities()
{
	if(GetOwner()->HasAuthority())
	{
		for(const FDefaultAbilitySpec& DefaultAbility : DefaultAbilities)
		{
			if(!IsValid(DefaultAbility.InitializationAbility))
			{
				continue;
			}
			
			GiveAbility(
				FGameplayAbilitySpec(DefaultAbility.InitializationAbility,
				                     GetStartingLevel(DefaultAbility),
				                     static_cast<int32>(DefaultAbility.InitializationAbility.GetDefaultObject()->GetInputId())
				)
			);
		}
	}
}

void URootAbilitySystemComponent::CollectAffectAttributeSets(const UGameplayEffect* Effect,
	TSet<TSubclassOf<UAttributeSet>>& AttributeSets)
{
	if(IsValid(Effect))
	{
		for(FGameplayModifierInfo Modifier : Effect->Modifiers)
		{
			if(Modifier.Attribute.IsValid())
			{
				TSubclassOf<UAttributeSet> SetClass = Modifier.Attribute.GetAttributeSetClass();
				if(*SetClass != nullptr)
				{
					AttributeSets.Add(SetClass);
				}
			}

			//TODO: Scrub modifiers from other fields if necessary. You can also override this function to add additional Sets
		}

	}
}

int URootAbilitySystemComponent::GetStartingLevel(const FDefaultAttributeSpec& DefaultAttributeSpec)
{
	return StartingLevel;
}

int URootAbilitySystemComponent::GetStartingLevel(const FDefaultAbilitySpec& DefaultAbilitySpec)
{
	return StartingLevel;
}
