#pragma once
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SpectatorPawn.h"

#include "RootSpectatorPawn.generated.h"

UCLASS()
class GGJ23_API ARootSpectatorPawn : public ASpectatorPawn, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GameplayTags;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer.AppendTags(GameplayTags);
	}
};
