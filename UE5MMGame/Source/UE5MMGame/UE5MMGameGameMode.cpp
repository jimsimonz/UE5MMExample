// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5MMGameGameMode.h"
#include "UE5MMGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5MMGameGameMode::AUE5MMGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
