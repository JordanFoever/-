#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class EASYSURVIVALRPGV2_5_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
		void ModifyChracterStates(int ValueBeforeModify,int Value,const FString& StateTag); // 修改人物的状态

	bool Initialize() override;

	UFUNCTION(BlueprintImplementableEvent) 
		void OnCharacterStyleChanged(const FString& Tag,const FString& StrContent);    // note=>UFUNCTION中函数的参数字符串类型必须为常量引用否则将会报错
};
