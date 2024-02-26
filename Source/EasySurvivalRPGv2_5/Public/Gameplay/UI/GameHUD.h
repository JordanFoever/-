// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUD.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
		UUserWidget* InteractiveInformation;              // 交互时显示的交互面板提示

	void SetUpInteractiveInfo(bool bShow, AActor* InteractiveActor, FString Title, FString Summary, FString Tip);

	UFUNCTION(BlueprintImplementableEvent)
		void SetInteractiveInfo(const FString& Title,const FString& Summary,const FString& Tip);    // note=> UFUNCTION中的函数参数要么是常量引用要么是普通引用

	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerInventorySlot(int32 Index);      // 用蓝图实现
	 
private:
	bool bInteractiveInfoShow;  // 是否显示交信息
	AActor* CurInteractiveActor;   // 交互的对象 

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;  // override from UUserWidget
	void UpdateInteractiveInfo();
};
