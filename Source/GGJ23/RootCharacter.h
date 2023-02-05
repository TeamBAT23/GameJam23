// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RootCharacter.generated.h"


UCLASS(Blueprintable, config=Game)
class GGJ23_API ARootCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	//Gameplay Ability System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta = (AllowPrivateAccess = "true"))
	class URootAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GameplayTags;
	
public:
	ARootCharacter();
	
protected:
	void PossessedBy(AController* NewController)
	{
		Super::PossessedBy(NewController);
		Server_GASInit();
	}

	void OnRep_PlayerState()
	{
		Super::OnRep_PlayerState();
		Client_GASInit();
	}
	
	//Gameplay Ability System Related
	//Ability System Related functionality
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void Server_GASInit();
	void Client_GASInit();
	void BindAbilityComponentToInputComponent() const;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer.AppendTags(GameplayTags);
	}

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
};