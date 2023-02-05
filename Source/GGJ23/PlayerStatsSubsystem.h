#pragma once

#include "PlayerStatsSubsystem.generated.h"

UCLASS(BlueprintType)
class UPlayerStatsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int PlayerMaxHealth = 100;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int PlayerCurrentHealth = 100;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int PlayerCurrentMoney = 0;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld)
	{
		PlayerCurrentHealth = PlayerMaxHealth;
	}


	UFUNCTION(BlueprintCallable)
	void SetHealth(int NewValue)
	{
		PlayerCurrentHealth = NewValue;

		if(PlayerCurrentHealth <= 0)
		{
			OnPlayerDie();	
		}
	}

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int NewValue)
	{
		int Delta = NewValue - PlayerMaxHealth;
		PlayerMaxHealth = NewValue;
		if(Delta > 0)
		{
			SetHealth(PlayerCurrentHealth + Delta);
		}
		else if(PlayerCurrentHealth < PlayerMaxHealth)
		{
			SetHealth(NewValue);
		}
	}

	UFUNCTION(BlueprintCallable)
	void SetMoney(int NewValue)
	{
		PlayerCurrentMoney = NewValue;
	}

	UFUNCTION(BlueprintCallable)
	void AddMoney(int Money)
	{
		PlayerCurrentMoney += Money;
	}

	/** @return false if not enough money to subtract*/
	UFUNCTION(BlueprintCallable)
	bool SubtractMoney(int Money)
	{
		if(PlayerCurrentMoney < Money)
		{
			return false;
		}
		PlayerCurrentMoney -= Money;
		return true;
	}

	UFUNCTION(BlueprintCallable)
	void OnPlayerDie()
	{
		//Trigger Game over screen
	}
	
};
