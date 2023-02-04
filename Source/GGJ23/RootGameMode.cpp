// Copyright Epic Games, Inc. All Rights Reserved.

#include "RootGameMode.h"
#include "RootCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARootGameMode::ARootGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
