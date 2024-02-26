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
		TMap<FName,int> StyleConfigs;                     // FName ���ݱ��е�ĳһ��, int ����ĸ����е�ĳһ��
};
