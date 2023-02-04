// Authored by Andrew Case

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RootGameplayAbility.h"
#include "GameplayEffect.h"
#include "GameplayEffect.h"
#include "RootAbilitySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct GGJ23_API FDefaultAttributeSpec
{
	GENERATED_BODY()
	
	/** Effect used to assign attributes to a Gameplay Ability Component at Spawn*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InitializationEffect;
};

USTRUCT(BlueprintType)
struct GGJ23_API FDefaultAbilitySpec
{
	GENERATED_BODY()
	
	/** Effect used to assign attributes to a Gameplay Ability Component at Spawn*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<URootGameplayAbility> InitializationAbility;
};


UCLASS(HideCategories=AttributeTest)
class GGJ23_API URootAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	virtual void OnRegister() override;
	
	/** Called once on client to initialize the component*/
	virtual void ClientInit();

	/** Called once on server to initialize the component*/
	virtual void ServerInit();

	/** Initialize Attributes to default values based on effects*/
	void GiveDefaultAttributes();

	/** Give any default abilities that the component should start with*/
	virtual void GiveDefaultAbilities();

protected:
	/** Get all attribute sets that a given GameplayEffect requires/references. Used to create Attribute Sets at startup.*/
	virtual void CollectAffectAttributeSets(const UGameplayEffect* Effect, TSet<TSubclassOf<UAttributeSet>>& AttributeSets);

	/**
	 * Get the starting level for a given attribute (effect).
	 * By default it just returns starting level,
	 * but it's possible to return different levels based on the default attribute spec
	 */
	virtual int GetStartingLevel(const FDefaultAttributeSpec& DefaultAttributeSpec);

	/**
	 * Get the starting level for a given ability
	 * By default it just returns starting level,
	 * but it's possible to return different levels based on the ability spec
	 */
	virtual int GetStartingLevel(const FDefaultAbilitySpec& DefaultAbilitySpec);

public:
	/** Attribute values to initialize the component to.*/
	UPROPERTY(EditAnywhere, Category="Initialization")
	TArray<FDefaultAttributeSpec> DefaultAttributes;

	/** Abilities to give to the component on startup*/
	UPROPERTY(EditAnywhere, Category="Initialization")
	TArray<FDefaultAbilitySpec> DefaultAbilities;

	/** Level to apply the attributes at on start. Can be used for scaling.*/
	UPROPERTY(EditAnywhere)
	int StartingLevel = 1;
};
