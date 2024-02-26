// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EASYSURVIVALRPGV2_5_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	void BeginPlay() override;
};
