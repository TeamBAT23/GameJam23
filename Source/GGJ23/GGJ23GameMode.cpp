// Copyright Epic Games, Inc. All Rights Reserved.

#include "GGJ23GameMode.h"
#include "GGJ23Character.h"
#include "UObject/ConstructorHelpers.h"

AGGJ23GameMode::AGGJ23GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
