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
		void ModifyChracterStates(int ValueBeforeModify,int Value,const FString& StateTag); // �޸������״̬

	bool Initialize() override;

	UFUNCTION(BlueprintImplementableEvent) 
		void OnCharacterStyleChanged(const FString& Tag,const FString& StrContent);    // note=>UFUNCTION�к����Ĳ����ַ������ͱ���Ϊ�������÷��򽫻ᱨ��
};
