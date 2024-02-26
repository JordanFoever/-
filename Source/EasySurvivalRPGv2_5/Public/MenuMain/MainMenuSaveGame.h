// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MainMenuSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class EASYSURVIVALRPGV2_5_API UMainMenuSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TMap<FName,int> StyleConfigs;                     // FName 数据表中的某一行, int 具体的该行中的某一个
};
